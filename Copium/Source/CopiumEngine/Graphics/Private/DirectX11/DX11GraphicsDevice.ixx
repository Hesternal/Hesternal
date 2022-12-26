module;

// NOTE(v.matushkin): Can't use ASSERT here, because Microsoft introduced new ICE after VS 17.4 preview 3.0?
//#include "Copium/Core/Debug.hpp"
#include "Copium/Core/Defines.hpp"

#include <d3d11_4.h>
#if COP_ENABLE_GRAPHICS_API_DEBUG
#include <dxgidebug.h>
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get import std;
COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
COP_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
export module CopiumEngine.Graphics.DX11GraphicsDevice;

import Copium.Core;

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
        ~DX11CommandBuffer();

        DX11CommandBuffer(DX11CommandBuffer&& other) = default;
        DX11CommandBuffer& operator=(DX11CommandBuffer&& other) = default;

        //- RenderPass
        void BeginRenderPass(RenderPassHandle renderPassHandle) override;

        void SetViewport(const Rect& viewportRect) override;
        void SetScissorRect(const RectInt& scissorRect) override;

        //- Bind
        void BindShader(ShaderHandle shaderHandle) override;
        void BindVertexBuffer(GraphicsBufferHandle vertexBufferHandle, uint32 stride, uint32 offset) override;
        void BindVertexBuffers(GraphicsBufferHandle vertexBufferHandle, const uint32 strides[3], const uint32 offsets[3]) override;
        void BindIndexBuffer(GraphicsBufferHandle indexBufferHandle, IndexFormat indexFormat) override;
        void BindConstantBuffer(GraphicsBufferHandle constantBufferHandle, uint32 slot) override;
        void BindConstantBuffer(GraphicsBufferHandle constantBufferHandle, uint32 slot, uint32 elementIndex, uint32 elementSize) override;
        void BindTexture(TextureHandle textureHandle, uint32 slot) override;
        void BindTexture(RenderTextureHandle renderTextureHandle, uint32 slot) override;
        void BindMaterial(TextureHandle baseColorTextureHandle, TextureHandle normalTextureHandle) override;

        //- Draw
        void DrawIndexed(uint32 indexCount, uint32 firstIndex, uint32 vertexOffset) override;
        void DrawProcedural(uint32 vertexCount) override;

        //- GraphicsBuffer
        void CopyBuffer(GraphicsBufferHandle srcGraphicsBufferHandle, GraphicsBufferHandle dstGraphicsBufferHandle) override;
        [[nodiscard]] void* MapBuffer(GraphicsBufferHandle graphicsBufferHandle) override;
        void UnmapBuffer(GraphicsBufferHandle graphicsBufferHandle) override;

#if COP_ENABLE_GRAPHICS_API_DEBUG
        void BeginSample(std::string_view name) override;
        void EndSample() override;
#endif

    private:
        ID3D11DeviceContext4* const m_deviceContext;
        DX11GraphicsDevice*   const m_graphicsDevice;

#if COP_ENABLE_GRAPHICS_API_DEBUG
        ID3DUserDefinedAnnotation* m_annotation;
        bool                       m_makeAnnotationCalls;
#endif
    };


    class DX11GraphicsDevice final : public IGraphicsDevice
    {
        friend DX11CommandBuffer;

    public:
        DX11GraphicsDevice();
        ~DX11GraphicsDevice();

        DX11GraphicsDevice(DX11GraphicsDevice&& other) noexcept = default;
        DX11GraphicsDevice& operator=(DX11GraphicsDevice&& other) noexcept = default;

        [[nodiscard]] RenderTextureHandle GetSwapchainRenderTexture(SwapchainHandle swapchainHandle) override;

        void BeginFrame() override;
        void EndFrame() override;

        [[nodiscard]] std::unique_ptr<ICommandBuffer> GetCommandBuffer() override;

        [[nodiscard]] GraphicsBufferHandle CreateGraphicsBuffer(const GraphicsBufferDesc& graphicsBufferDesc, std::span<const uint8> initialData) override;
        [[nodiscard]] RenderPassHandle CreateRenderPass(const RenderPassDesc& renderPassDesc) override;
        [[nodiscard]] RenderTextureHandle CreateRenderTexture(const RenderTextureDesc& renderTextureDesc) override;
        [[nodiscard]] ShaderHandle CreateShader(const ShaderDesc& shaderDesc) override;
        [[nodiscard]] SwapchainHandle CreateSwapchain(const SwapchainDesc& swapchainDesc) override;
        [[nodiscard]] TextureHandle CreateTexture2D(const TextureDesc& textureDesc, const SamplerDesc& samplerDesc) override;

        void ResizeSwapchain(SwapchainHandle swapchainHandle, uint16 width, uint16 height) override;

        void DestroyGraphicsBuffer(GraphicsBufferHandle graphicsBufferHandle) override;
        void DestroyRenderPass(RenderPassHandle renderPassHandle) override;
        void DestroyRenderTexture(RenderTextureHandle renderTextureHandle) override;
        void DestroyShader(ShaderHandle shaderHandle) override;
        void DestroySwapchain(SwapchainHandle swapchainHandle) override;
        void DestroyTexture2D(TextureHandle textureHandle) override;

    private:
        [[nodiscard]] ID3D11SamplerState* GetRenderTextureSampler() const { return m_renderTextureSampler; }

        [[nodiscard]] DX11GraphicsBuffer& _GetGraphicsBuffer(GraphicsBufferHandle graphicsBufferHandle);
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

        ID3D11SamplerState*   m_renderTextureSampler;

        std::unordered_map<GraphicsBufferHandle, DX11GraphicsBuffer> m_graphicsBuffers;
        std::unordered_map<RenderPassHandle,     DX11RenderPass>     m_renderPasses;
        std::unordered_map<RenderTextureHandle,  DX11RenderTexture>  m_renderTextures;
        std::unordered_map<ShaderHandle,         DX11Shader>         m_shaders;
        std::unordered_map<SwapchainHandle,      DX11Swapchain>      m_swapchains;
        std::unordered_map<TextureHandle,        DX11Texture2D>      m_textures;
    };


    DX11GraphicsBuffer& DX11GraphicsDevice::_GetGraphicsBuffer(GraphicsBufferHandle graphicsBufferHandle)
    {
        const auto dx11GraphicsBufferIterator = m_graphicsBuffers.find(graphicsBufferHandle);
        // TODO(v.matushkin): <ICE/MixingHeadersAndHeaderUnits>
        //COP_ASSERT(dx11GraphicsBufferIterator != m_graphicsBuffers.end());
#if COP_ENABLE_ASSERTS
        if (dx11GraphicsBufferIterator == m_graphicsBuffers.end())
        {
            std::abort();
        }
#endif

        return dx11GraphicsBufferIterator->second;
    }

    DX11RenderPass& DX11GraphicsDevice::_GetRenderPass(RenderPassHandle renderPassHandle)
    {
        const auto dx11RenderPassIterator = m_renderPasses.find(renderPassHandle);
        // TODO(v.matushkin): <ICE/MixingHeadersAndHeaderUnits>
        //COP_ASSERT(dx11RenderPassIterator != m_renderPasses.end());
#if COP_ENABLE_ASSERTS
        if (dx11RenderPassIterator == m_renderPasses.end())
        {
            std::abort();
        }
#endif

        return dx11RenderPassIterator->second;
    }

    DX11RenderTexture& DX11GraphicsDevice::_GetRenderTexture(RenderTextureHandle renderTextureHandle)
    {
        const auto dx11RenderTextureIterator = m_renderTextures.find(renderTextureHandle);
        // TODO(v.matushkin): <ICE/MixingHeadersAndHeaderUnits>
        //COP_ASSERT(dx11RenderTextureIterator != m_renderTextures.end());
#if COP_ENABLE_ASSERTS
        if (dx11RenderTextureIterator == m_renderTextures.end())
        {
            std::abort();
        }
#endif

        return dx11RenderTextureIterator->second;
    }

    DX11Shader& DX11GraphicsDevice::_GetShader(ShaderHandle shaderHandle)
    {
        const auto dx11ShaderIterator = m_shaders.find(shaderHandle);
        // TODO(v.matushkin): <ICE/MixingHeadersAndHeaderUnits>
        //COP_ASSERT(dx11ShaderIterator != m_shaders.end());
#if COP_ENABLE_ASSERTS
        if (dx11ShaderIterator == m_shaders.end())
        {
            std::abort();
        }
#endif

        return dx11ShaderIterator->second;
    }

    DX11Swapchain& DX11GraphicsDevice::_GetSwapchain(SwapchainHandle swapchainHandle)
    {
        const auto dx11SwapchainIterator = m_swapchains.find(swapchainHandle);
        // TODO(v.matushkin): <ICE/MixingHeadersAndHeaderUnits>
        //COP_ASSERT(dx11SwapchainIterator != m_swapchains.end());
#if COP_ENABLE_ASSERTS
        if (dx11SwapchainIterator == m_swapchains.end())
        {
            std::abort();
        }
#endif

        return dx11SwapchainIterator->second;
    }

    DX11Texture2D& DX11GraphicsDevice::_GetTexture(TextureHandle textureHandle)
    {
        const auto dx11TextureIterator = m_textures.find(textureHandle);
        // TODO(v.matushkin): <ICE/MixingHeadersAndHeaderUnits>
        //COP_ASSERT(dx11TextureIterator != m_textures.end());
#if COP_ENABLE_ASSERTS
        if (dx11TextureIterator == m_textures.end())
        {
            std::abort();
        }
#endif

        return dx11TextureIterator->second;
    }

} // export namespace Copium
