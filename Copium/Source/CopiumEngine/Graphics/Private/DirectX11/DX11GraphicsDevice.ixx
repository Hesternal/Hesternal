module;

#include "Engine/Core/Defines.hpp"

#include <d3d11_4.h>
#if COP_ENABLE_GRAPHICS_API_DEBUG
#include <dxgidebug.h>
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get import std;
COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
COP_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
export module CopiumEngine.Graphics.DX11GraphicsDevice;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Graphics.IGraphicsDevice;

import <unordered_map>;
import <vector>;
COP_WARNING_POP


export namespace Copium
{

    class DX11GraphicsDevice final : public IGraphicsDevice
    {
        static constexpr uint8 k_RenderPassColorAttachments = 8;


        struct DX11GraphicsBuffer
        {
            ID3D11Buffer* Buffer;

            void Release();
        };

        struct DX11Mesh
        {
            ID3D11Buffer* Index;
            ID3D11Buffer* VertexBuffers[3];
            DXGI_FORMAT   IndexFormat;
            uint32        IndexCount;
            uint32        VertexStrides[3];
            uint32        VertexOffsets[3];

            void Release();
        };

        struct DX11RenderTexture
        {
            ID3D11Texture2D1*          Texture;
            // NOTE(v.matushkin): RTV and DSV should be in a union or std::variant, but it turns into a fuckin mess,
            //  so fuck it, it's easier to leave it like this
            ID3D11RenderTargetView1*   RTV;
            ID3D11DepthStencilView*    DSV;
            ID3D11ShaderResourceView1* SRV;
            RenderTextureClearValue    ClearValue;
            RenderTextureType          Type;

            void Release();
        };

        struct DX11Subpass
        {
            RenderTextureHandle ColorAttachments[k_RenderPassColorAttachments];
            RenderTextureHandle DepthStencilAttachment;
            uint8               NumColorAttachments;

            // std::vector<ID3D11RenderTargetView*> RTVs;
            // ID3D11DepthStencilView*              DSV;
        };

        // NOTE(v.matushkin): I can't just save ID3D11RenderTargetView/ID3D11DepthStencilView because
        //  RenderTexture can change (eg. Swapchain resize).
        struct DX11RenderPass
        {
            RenderTextureHandle ClearColorAttachments[k_RenderPassColorAttachments];
            RenderTextureHandle ClearDepthStencilAttachment;
            uint8               NumClearColorAttachments;
            DX11Subpass         Subpass;

            // std::vector<ID3D11RenderTargetView*> ClearRTVs;
            // std::vector<ClearColorValue>         ClearColorValues;
            // ID3D11DepthStencilView*              ClearDSV;
            // ClearDepthStencilValue               ClearDepthStencilValue;
            // uint32                               DepthStencilClearFlags;
        };

        struct DX11Shader
        {
            ID3D11InputLayout*       InputLayout;
            ID3D11VertexShader*      VertexShader;
            ID3D11RasterizerState2*  RasterizerState;
            ID3D11PixelShader*       PixelShader;
            ID3D11DepthStencilState* DepthStencilState;
            ID3D11BlendState1*       BlendState;

            void Release();
        };

        struct DX11Swapchain
        {
            IDXGISwapChain4*     Swapchain;
            DXGI_FORMAT          Format;
            uint32               Flags; // DXGI_SWAP_CHAIN_FLAG
            uint8                BufferCount;
            RenderTextureHandle  SwapchainRTHandle;

            void Release(DX11GraphicsDevice* dx11GraphicsDevice);
        };

        struct DX11Texture2D
        {
            ID3D11Texture2D1*          Texture;
            ID3D11ShaderResourceView1* SRV;
            ID3D11SamplerState*        Sampler;

            void Release();
        };

        struct PerCamera
        {
            Float4x4 _CameraView;
            Float4x4 _CameraProjection;
        };

    public:
        DX11GraphicsDevice();
        ~DX11GraphicsDevice();

        DX11GraphicsDevice(DX11GraphicsDevice&& other) noexcept = default;
        DX11GraphicsDevice& operator=(DX11GraphicsDevice&& other) noexcept = default;

        //< For ImGui
        // ID3D11ShaderResourceView*
        [[nodiscard]] void* GetNativeRenderTexture(RenderTextureHandle renderTextureHandle) override;
        //> For ImGui
        [[nodiscard]] RenderTextureHandle GetSwapchainRenderTexture(SwapchainHandle swapchainHandle) override;

        // TODO(v.matushkin): <ImGui/CustomBackend>
        [[nodiscard]] ID3D11Device* GetDevice() const { return m_device; }
        [[nodiscard]] ID3D11DeviceContext* GetDeviceContext() const { return m_deviceContext; }

        void BeginFrame(const Float4x4& objectToWorld, const Float4x4& cameraView, const Float4x4& cameraProjection) override;
        void BeginRenderPass(RenderPassHandle renderPassHandle) override;
        void EndFrame() override;

        void BindShader(ShaderHandle shaderHandle) override;
        void BindMaterial(TextureHandle baseColorTextureHandle, TextureHandle normalTextureHandle) override;

        void DrawMesh(MeshHandle meshHandle) override;
        void DrawProcedural(uint32 vertexCount) override;

        [[nodiscard]] GraphicsBufferHandle CreateGraphicsBuffer(const GraphicsBufferDesc& graphicsBufferDesc, std::span<const uint8> initialData) override;
        [[nodiscard]] MeshHandle CreateMesh(const MeshDesc& meshDesc) override;
        [[nodiscard]] RenderPassHandle CreateRenderPass(const RenderPassDesc& renderPassDesc) override;
        [[nodiscard]] RenderTextureHandle CreateRenderTexture(const RenderTextureDesc& renderTextureDesc) override;
        [[nodiscard]] ShaderHandle CreateShader(const ShaderDesc& shaderDesc) override;
        [[nodiscard]] SwapchainHandle CreateSwapchain(const SwapchainDesc& swapchainDesc) override;
        [[nodiscard]] TextureHandle CreateTexture2D(const TextureDesc& textureDesc) override;

        void UpdateGraphicsBuffer(GraphicsBufferHandle graphicsBufferHandle, std::span<const uint8> data) override;
        void ResizeSwapchain(SwapchainHandle swapchainHandle, uint16 width, uint16 height) override;

        void DestroyGraphicsBuffer(GraphicsBufferHandle graphicsBufferHandle) override;
        void DestroyMesh(MeshHandle meshHandle) override;
        void DestroyRenderPass(RenderPassHandle renderPassHandle) override;
        void DestroyRenderTexture(RenderTextureHandle renderTextureHandle) override;
        void DestroyShader(ShaderHandle shaderHandle) override;
        void DestroySwapchain(SwapchainHandle swapchainHandle) override;
        void DestroyTexture2D(TextureHandle textureHandle) override;

    private:
        void _CreateDevice();

#if COP_ENABLE_GRAPHICS_API_DEBUG
        void _DebugLayer_Init();
        void _DebugLayer_ReportLiveObjects();
        void _DebugLayer_LogMessages();
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

    private:
        IDXGIFactory5*        m_factory;
        ID3D11Device5*        m_device;
        ID3D11DeviceContext4* m_deviceContext;

#if COP_ENABLE_GRAPHICS_API_DEBUG
        IDXGIInfoQueue*       m_dxgiInfoQueue;
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

        ID3D11Buffer*         m_cbPerCamera;
        ID3D11Buffer*         m_cbPerMesh;

        std::unordered_map<MeshHandle,          DX11Mesh>          m_meshes;
        std::unordered_map<RenderPassHandle,    DX11RenderPass>    m_renderPasses;
        std::unordered_map<GraphicsBufferHandle, DX11GraphicsBuffer> m_graphicsBuffers;
        std::unordered_map<MeshHandle,           DX11Mesh>           m_meshes;
        std::unordered_map<RenderPassHandle,     DX11RenderPass>     m_renderPasses;
        std::unordered_map<RenderTextureHandle,  DX11RenderTexture>  m_renderTextures;
        std::unordered_map<ShaderHandle,         DX11Shader>         m_shaders;
        std::unordered_map<SwapchainHandle,      DX11Swapchain>      m_swapchains;
        std::unordered_map<TextureHandle,        DX11Texture2D>      m_textures;
    };

} // export namespace Copium
