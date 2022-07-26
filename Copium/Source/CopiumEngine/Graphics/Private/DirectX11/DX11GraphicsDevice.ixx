module;

#include "Engine/Core/Defines.hpp"

#include <d3d11_4.h>
#if COP_ENABLE_GRAPHICS_API_DEBUG
#include <dxgidebug.h>
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

#include <unordered_map>

export module CopiumEngine.Graphics.DX11GraphicsDevice;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Graphics.IGraphicsDevice;


export namespace Copium
{

    class DX11GraphicsDevice final : public IGraphicsDevice
    {
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
            ID3D11Texture2D1*         Texture;
            // NOTE(v.matushkin): RTV and DSV should be in a union or std::variant, but it turns into a fuckin mess,
            //  so fuck it, it's easier to leave it like this
            ID3D11RenderTargetView*   Rtv;
            ID3D11DepthStencilView*   Dsv;
            ID3D11ShaderResourceView* Srv;        // NOTE(v.matushkin): Can be null
            RenderTextureClearValue   ClearValue;
            RenderTextureType         Type;       // NOTE(v.matushkin): May be store DepthStencilClearFlags instead of type?

            void Release();
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
            IDXGISwapChain4*        Swapchain;
            ID3D11Texture2D1*       Texture;     // NOTE(v.matushkin): No need to save it?
            ID3D11RenderTargetView* Rtv;
            DXGI_FORMAT             Format;
            DXGI_SWAP_CHAIN_FLAG    Flags;
            uint8                   BufferCount;

            void Release();
        };

        struct DX11Texture2D
        {
            ID3D11Texture2D1*         Texture;
            ID3D11ShaderResourceView* Srv;
            ID3D11SamplerState*       Sampler;

            void Release();
        };

        struct PerCamera
        {
            Matrix4x4f _CameraView;
            Matrix4x4f _CameraProjection;
        };

    public:
        DX11GraphicsDevice();
        ~DX11GraphicsDevice();

        DX11GraphicsDevice(DX11GraphicsDevice&& other) noexcept = default;
        DX11GraphicsDevice& operator=(DX11GraphicsDevice&& other) noexcept = default;

        void BeginFrame(const Matrix4x4f& objectToWorld, const Matrix4x4f& cameraView, const Matrix4x4f& cameraProjection) override;
        void EndFrame() override;

        void BindShader(ShaderHandle shaderHandle) override;
        void BindMaterial(TextureHandle baseColorTextureHandle, TextureHandle normalTextureHandle) override;

        void DrawMesh(MeshHandle meshHandle) override;
        void DrawProcedural(uint32 vertexCount) override;

        [[nodiscard]] MeshHandle CreateMesh(const MeshDesc& meshDesc) override;
        [[nodiscard]] RenderTextureHandle CreateRenderTexture(const RenderTextureDesc& renderTextureDesc) override;
        [[nodiscard]] ShaderHandle CreateShader(const ShaderDesc& shaderDesc) override;
        [[nodiscard]] SwapchainHandle CreateSwapchain(const SwapchainDesc& swapchainDesc) override;
        [[nodiscard]] TextureHandle CreateTexture2D(const TextureDesc& textureDesc) override;

        void ResizeSwapchain(SwapchainHandle swapchainHandle, uint16 width, uint16 height) override;

        void DestroyMesh(MeshHandle meshHandle) override;
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

        RenderTextureHandle   m_depthRenderTextureHandle;

        std::unordered_map<MeshHandle,          DX11Mesh>          m_meshes;
        std::unordered_map<RenderTextureHandle, DX11RenderTexture> m_renderTextures;
        std::unordered_map<ShaderHandle,        DX11Shader>        m_shaders;
        std::unordered_map<SwapchainHandle,     DX11Swapchain>     m_swapchains;
        std::unordered_map<TextureHandle,       DX11Texture2D>     m_textures;
    };

} // export namespace Copium
