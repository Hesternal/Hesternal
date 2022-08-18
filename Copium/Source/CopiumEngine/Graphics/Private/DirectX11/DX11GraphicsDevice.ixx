module;

#include "Engine/Core/Debug.hpp"

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
import CopiumEngine.Graphics.ICommandBuffer;
import CopiumEngine.Graphics.IGraphicsDevice;

import <unordered_map>;
import <vector>;
COP_WARNING_POP


namespace Copium
{

    class DX11GraphicsDevice;


    inline constexpr uint8 k_RenderPassColorAttachments = 8;


    struct DX11GraphicsBuffer final
    {
        ID3D11Buffer* Buffer;

        void Release();
    };

    struct DX11Mesh final
    {
        ID3D11Buffer* Index;
        ID3D11Buffer* VertexBuffers[3];
        DXGI_FORMAT   IndexFormat;
        uint32        IndexCount;
        uint32        VertexStrides[3];
        uint32        VertexOffsets[3];

        void Release();
    };

    struct DX11RenderTexture final
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

    struct DX11Subpass final
    {
        RenderTextureHandle ColorAttachments[k_RenderPassColorAttachments];
        RenderTextureHandle DepthStencilAttachment;
        uint8               NumColorAttachments;

        // std::vector<ID3D11RenderTargetView*> RTVs;
        // ID3D11DepthStencilView*              DSV;
    };

    // NOTE(v.matushkin): I can't just save ID3D11RenderTargetView/ID3D11DepthStencilView because
    //  RenderTexture can change (eg. Swapchain resize).
    struct DX11RenderPass final
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

    struct DX11Shader final
    {
        ID3D11InputLayout*       InputLayout;
        ID3D11VertexShader*      VertexShader;
        ID3D11RasterizerState2*  RasterizerState;
        ID3D11PixelShader*       PixelShader;
        ID3D11DepthStencilState* DepthStencilState;
        ID3D11BlendState1*       BlendState;

        void Release();
    };

    struct DX11Swapchain final
    {
        IDXGISwapChain4*    Swapchain;
        DXGI_FORMAT         Format;
        uint32              Flags; // DXGI_SWAP_CHAIN_FLAG
        uint8               BufferCount;
        RenderTextureHandle SwapchainRTHandle;

        void Release(DX11GraphicsDevice* dx11GraphicsDevice);
    };

    struct DX11Texture2D final
    {
        ID3D11Texture2D1*          Texture;
        ID3D11ShaderResourceView1* SRV;
        ID3D11SamplerState*        Sampler;

        void Release();
    };

} // namespace Copium


export namespace Copium
{

    class DX11CommandBuffer final : public ICommandBuffer
    {
    public:
        DX11CommandBuffer(ID3D11DeviceContext4* deviceContext, DX11GraphicsDevice* graphicsDevice);
        ~DX11CommandBuffer() = default;

        DX11CommandBuffer(DX11CommandBuffer&& other) = default;
        DX11CommandBuffer& operator=(DX11CommandBuffer&& other) = default;

        void BeginRenderPass(RenderPassHandle renderPassHandle) override;

        void SetViewport(const Rect& viewportRect) override;
        void SetScissorRect(const RectInt& scissorRect) override;

        void BindShader(ShaderHandle shaderHandle) override;
        void BindVertexBuffer(GraphicsBufferHandle vertexBufferHandle, uint32 stride, uint32 offset) override;
        void BindIndexBuffer(GraphicsBufferHandle indexBufferHandle, IndexFormat indexFormat) override;
        void BindConstantBuffer(GraphicsBufferHandle constantBufferHandle, uint32 slot) override;
        void BindTexture(TextureHandle textureHandle, uint32 slot) override;
        void BindTexture(RenderTextureHandle renderTextureHandle, uint32 slot) override;
        void BindMaterial(TextureHandle baseColorTextureHandle, TextureHandle normalTextureHandle) override;

        void DrawIndexed(uint32 indexCount, uint32 firstIndex, uint32 vertexOffset) override;
        void DrawMesh(MeshHandle meshHandle) override;
        void DrawProcedural(uint32 vertexCount) override;

    private:
        ID3D11DeviceContext4* const m_deviceContext;
        DX11GraphicsDevice*   const m_graphicsDevice;
    };


    class DX11GraphicsDevice final : public IGraphicsDevice
    {
        friend DX11CommandBuffer;


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

        [[nodiscard]] RenderTextureHandle GetSwapchainRenderTexture(SwapchainHandle swapchainHandle) override;

        void BeginFrame(const Float4x4& objectToWorld, const Float4x4& cameraView, const Float4x4& cameraProjection) override;
        void EndFrame() override;

        [[nodiscard]] std::unique_ptr<ICommandBuffer> GetCommandBuffer() override;

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
        [[nodiscard]] ID3D11SamplerState* GetRenderTextureSampler() const { return m_renderTextureSampler; }

        [[nodiscard]] DX11GraphicsBuffer& _GetGraphicsBuffer(GraphicsBufferHandle graphicsBufferHandle);
        [[nodiscard]] DX11Mesh& _GetMesh(MeshHandle meshHandle);
        [[nodiscard]] DX11RenderPass& _GetRenderPass(RenderPassHandle renderPassHandle);
        [[nodiscard]] DX11RenderTexture& _GetRenderTexture(RenderTextureHandle renderTextureHandle);
        [[nodiscard]] DX11Shader& _GetShader(ShaderHandle shaderHandle);
        [[nodiscard]] DX11Swapchain& _GetSwapchain(SwapchainHandle swapchainHandle);
        [[nodiscard]] DX11Texture2D& _GetTexture(TextureHandle textureHandle);

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

        ID3D11SamplerState*   m_renderTextureSampler;

        std::unordered_map<GraphicsBufferHandle, DX11GraphicsBuffer> m_graphicsBuffers;
        std::unordered_map<MeshHandle,           DX11Mesh>           m_meshes;
        std::unordered_map<RenderPassHandle,     DX11RenderPass>     m_renderPasses;
        std::unordered_map<RenderTextureHandle,  DX11RenderTexture>  m_renderTextures;
        std::unordered_map<ShaderHandle,         DX11Shader>         m_shaders;
        std::unordered_map<SwapchainHandle,      DX11Swapchain>      m_swapchains;
        std::unordered_map<TextureHandle,        DX11Texture2D>      m_textures;
    };


    DX11GraphicsBuffer& DX11GraphicsDevice::_GetGraphicsBuffer(GraphicsBufferHandle graphicsBufferHandle)
    {
        const auto dx11GraphicsBufferIterator = m_graphicsBuffers.find(graphicsBufferHandle);
        COP_ASSERT(dx11GraphicsBufferIterator != m_graphicsBuffers.end());

        return dx11GraphicsBufferIterator->second;
    }

    DX11Mesh& DX11GraphicsDevice::_GetMesh(MeshHandle meshHandle)
    {
        const auto dx11MeshIterator = m_meshes.find(meshHandle);
        COP_ASSERT(dx11MeshIterator != m_meshes.end());

        return dx11MeshIterator->second;
    }

    DX11RenderPass& DX11GraphicsDevice::_GetRenderPass(RenderPassHandle renderPassHandle)
    {
        const auto dx11RenderPassIterator = m_renderPasses.find(renderPassHandle);
        COP_ASSERT(dx11RenderPassIterator != m_renderPasses.end());

        return dx11RenderPassIterator->second;
    }

    DX11RenderTexture& DX11GraphicsDevice::_GetRenderTexture(RenderTextureHandle renderTextureHandle)
    {
        const auto dx11RenderTextureIterator = m_renderTextures.find(renderTextureHandle);
        COP_ASSERT(dx11RenderTextureIterator != m_renderTextures.end());

        return dx11RenderTextureIterator->second;
    }

    DX11Shader& DX11GraphicsDevice::_GetShader(ShaderHandle shaderHandle)
    {
        const auto dx11ShaderIterator = m_shaders.find(shaderHandle);
        COP_ASSERT(dx11ShaderIterator != m_shaders.end());

        return dx11ShaderIterator->second;
    }

    DX11Swapchain& DX11GraphicsDevice::_GetSwapchain(SwapchainHandle swapchainHandle)
    {
        const auto dx11SwapchainIterator = m_swapchains.find(swapchainHandle);
        COP_ASSERT(dx11SwapchainIterator != m_swapchains.end());

        return dx11SwapchainIterator->second;
    }

    DX11Texture2D& DX11GraphicsDevice::_GetTexture(TextureHandle textureHandle)
    {
        const auto dx11TextureIterator = m_textures.find(textureHandle);
        COP_ASSERT(dx11TextureIterator != m_textures.end());

        return dx11TextureIterator->second;
    }

} // export namespace Copium
