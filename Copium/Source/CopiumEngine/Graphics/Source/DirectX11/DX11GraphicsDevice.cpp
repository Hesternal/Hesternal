module;

#include "Engine/Core/Debug.hpp"

#include <d3d11_4.h>
#include <d3dcompiler.h>
#if COP_ENABLE_GRAPHICS_API_DEBUG
#include <dxgidebug.h>
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get import std;
COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
COP_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
module CopiumEngine.Graphics.DX11GraphicsDevice;
COP_WARNING_POP

import CopiumEngine.Engine.EngineSettings;
import CopiumEngine.Graphics.DXCommon;

import <cstring>;
import <memory>;
import <string>;
import <unordered_map>;
import <utility>;


// TODO(v.matushkin):
//  - <Viewport>
//    Right now I call ID3D11DeviceContext4::RSSetViewports on swapchain resize/creation
//    Not sure that it should be like this. Probably a Camera should manage its viewport.
//
//  - <Device/Lost>
//    Handle ID3D11Device5::RegisterDeviceRemovedEvent


#define RELEASE_COM_PTR(ptr) if (ptr != nullptr) ptr->Release()

// NOTE(v.matushkin): May be add macro for FAILED(HRESULT) ?
//  Because right now I only log in EndFrame() and ~DX11GraphicsDevice()
//  What if some d3d/dxgi call failed earlier(than EndFrame() call) and the app crashed
#if COP_ENABLE_GRAPHICS_API_DEBUG
#define COP_DX11_LOG_DEBUG_LAYER() _DebugLayer_LogMessages()
#else
#define COP_DX11_LOG_DEBUG_LAYER() do { } while((void)0,0)
#endif // COP_ENABLE_GRAPHICS_API_DEBUG


namespace
{
    using namespace Copium;


    static constexpr float32 k_ViewportMinDepth = 0.0f;
    static constexpr float32 k_ViewportMaxDepth = 1.0f;


#if COP_ENABLE_GRAPHICS_API_DEBUG
    static const GUID k_DxgiDebugGuid = DXGI_DEBUG_ALL;
#endif // COP_ENABLE_GRAPHICS_API_DEBUG


    static constinit uint32 g_GraphicsBufferHandleWorkaround = 0;
    static constinit uint32 g_MeshHandleWorkaround           = 0;
    static constinit uint32 g_RenderPassHandleWorkaround     = 0;
    static constinit uint32 g_RenderTextureHandleWorkaround  = 0;
    static constinit uint32 g_ShaderHandleWorkaround         = 0;
    static constinit uint32 g_SwapchainHandleWorkaround      = 0;
    static constinit uint32 g_TextureHandleWorkaround        = 0;


    //- GraphicsBuffer
    [[nodiscard]] static uint32 dx11_GraphicsBufferBindFlags(GraphicsBufferUsage graphicsBufferUsage)
    {
        static const D3D11_BIND_FLAG d3dBindFlags[] = {
            D3D11_BIND_VERTEX_BUFFER,
            D3D11_BIND_INDEX_BUFFER,
            D3D11_BIND_CONSTANT_BUFFER,
        };

        return d3dBindFlags[std::to_underlying(graphicsBufferUsage)];
    }

    //- Mesh
    [[nodiscard]] static DXGI_FORMAT dx11_IndexFormat(IndexFormat indexFormat)
    {
        return indexFormat == IndexFormat::UInt16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    }

    //- RenderTexture
    #define DX11_NO_DEPTH_STENCIL 0
    [[nodiscard]] static uint32 dx11_RenderTextureTypeToClearFlags(RenderTextureType renderTextureType)
    {
        static const uint32 d3dClearFlags[] = {
            DX11_NO_DEPTH_STENCIL,
            D3D11_CLEAR_DEPTH,
            // D3D11_CLEAR_STENCIL,
            // D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        };

        return d3dClearFlags[std::to_underlying(renderTextureType)];
    }

    //- Texture
    [[nodiscard]] static D3D11_FILTER dx11_TextureFilterMode(TextureFilterMode textureFilterMode)
    {
        static const D3D11_FILTER d3dTextureFilterMode[] = {
            D3D11_FILTER_MIN_MAG_MIP_POINT,
            D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
            D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            D3D11_FILTER_ANISOTROPIC,
        };

        return d3dTextureFilterMode[std::to_underlying(textureFilterMode)];
    }

    [[nodiscard]] static D3D11_TEXTURE_ADDRESS_MODE dx11_TextureWrapMode(TextureWrapMode textureWrapMode)
    {
        static const D3D11_TEXTURE_ADDRESS_MODE d3dTextureWrapMode[] = {
            D3D11_TEXTURE_ADDRESS_CLAMP,
            D3D11_TEXTURE_ADDRESS_BORDER,
            D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,
            D3D11_TEXTURE_ADDRESS_MIRROR,
            D3D11_TEXTURE_ADDRESS_WRAP,
        };

        return d3dTextureWrapMode[std::to_underlying(textureWrapMode)];
    }

    //- Shader states

    //-- RasterizerState
    [[nodiscard]] static D3D11_CULL_MODE dx11_CullMode(CullMode cullMode)
    {
        static const D3D11_CULL_MODE d3dCullMode[] = {
            D3D11_CULL_NONE,
            D3D11_CULL_FRONT,
            D3D11_CULL_BACK,
        };

        return d3dCullMode[std::to_underlying(cullMode)];
    }

    [[nodiscard]] static D3D11_FILL_MODE dx11_PolygonMode(PolygonMode polygonMode)
    {
        static const D3D11_FILL_MODE d3dPolygonMode[] = {
            D3D11_FILL_SOLID,
            D3D11_FILL_WIREFRAME,
        };

        return d3dPolygonMode[std::to_underlying(polygonMode)];
    }

    //-- DepthStencilState
    [[nodiscard]] static D3D11_COMPARISON_FUNC dx11_CompareFunction(CompareFunction compareFunction)
    {
        static const D3D11_COMPARISON_FUNC d3dCompareFunction[] = {
            D3D11_COMPARISON_NEVER,
            D3D11_COMPARISON_LESS,
            D3D11_COMPARISON_EQUAL,
            D3D11_COMPARISON_LESS_EQUAL,
            D3D11_COMPARISON_GREATER,
            D3D11_COMPARISON_NOT_EQUAL,
            D3D11_COMPARISON_GREATER_EQUAL,
            D3D11_COMPARISON_ALWAYS,
        };

        return d3dCompareFunction[std::to_underlying(compareFunction)];
    }

    //-- BlendState
    [[nodiscard]] static D3D11_BLEND_OP dx11_BlendOp(BlendOp blendOp)
    {
        static const D3D11_BLEND_OP d3dBlendOp[] = {
            D3D11_BLEND_OP_ADD,
            D3D11_BLEND_OP_SUBTRACT,
            D3D11_BLEND_OP_REV_SUBTRACT,
            D3D11_BLEND_OP_MIN,
            D3D11_BLEND_OP_MAX,
        };

        return d3dBlendOp[std::to_underlying(blendOp)];
    }

    [[nodiscard]] static D3D11_BLEND dx11_BlendFactor(BlendFactor blendFactor)
    {
        static const D3D11_BLEND d3dBlendFactor[] = {
            D3D11_BLEND_ZERO,
            D3D11_BLEND_ONE,
            D3D11_BLEND_SRC_COLOR,
            D3D11_BLEND_INV_SRC_COLOR,
            D3D11_BLEND_DEST_COLOR,
            D3D11_BLEND_INV_DEST_COLOR,
            D3D11_BLEND_SRC_ALPHA,
            D3D11_BLEND_INV_SRC_ALPHA,
            D3D11_BLEND_DEST_ALPHA,
            D3D11_BLEND_INV_DEST_ALPHA,
            D3D11_BLEND_SRC_ALPHA_SAT,
            D3D11_BLEND_SRC1_COLOR,
            D3D11_BLEND_INV_SRC1_COLOR,
            D3D11_BLEND_SRC1_ALPHA,
            D3D11_BLEND_INV_SRC1_ALPHA,
        };

        return d3dBlendFactor[std::to_underlying(blendFactor)];
    }

    [[nodiscard]] static D3D11_LOGIC_OP dx11_BlendLogicOp(BlendLogicOp blendLogicOp)
    {
        static const D3D11_LOGIC_OP d3dBlendLogicOp[] = {
            D3D11_LOGIC_OP_CLEAR,
            D3D11_LOGIC_OP_SET,
            D3D11_LOGIC_OP_COPY,
            D3D11_LOGIC_OP_COPY_INVERTED,
            D3D11_LOGIC_OP_NOOP,
            D3D11_LOGIC_OP_INVERT,
            D3D11_LOGIC_OP_AND,
            D3D11_LOGIC_OP_NAND,
            D3D11_LOGIC_OP_OR,
            D3D11_LOGIC_OP_NOR,
            D3D11_LOGIC_OP_XOR,
            D3D11_LOGIC_OP_EQUIV,
            D3D11_LOGIC_OP_AND_REVERSE,
            D3D11_LOGIC_OP_AND_INVERTED,
            D3D11_LOGIC_OP_OR_REVERSE,
            D3D11_LOGIC_OP_OR_INVERTED,
        };

        return d3dBlendLogicOp[std::to_underlying(blendLogicOp)];
    }

} // namespace


namespace Copium
{

    void DX11GraphicsDevice::DX11GraphicsBuffer::Release()
    {
        Buffer->Release();
    }

    void DX11GraphicsDevice::DX11Mesh::Release()
    {
        Index->Release();
        VertexBuffers[0]->Release();
        VertexBuffers[1]->Release();
        VertexBuffers[2]->Release();
    }

    void DX11GraphicsDevice::DX11RenderTexture::Release()
    {
        Texture->Release();
        RELEASE_COM_PTR(RTV);
        RELEASE_COM_PTR(DSV);
        RELEASE_COM_PTR(SRV);
    }

    void DX11GraphicsDevice::DX11Shader::Release()
    {
        InputLayout->Release();
        VertexShader->Release();
        RasterizerState->Release();
        PixelShader->Release();
        DepthStencilState->Release();
        BlendState->Release();
    }

    void DX11GraphicsDevice::DX11Swapchain::Release(DX11GraphicsDevice* dx11GraphicsDevice)
    {
        dx11GraphicsDevice->DestroyRenderTexture(SwapchainRTHandle);

        // NOTE(v.matushkin): IDXGISwapChain->Release() can lead to errors if swapchain will be recreated from the same HWND right after this call,
        //  because swapchain destruction is deffered?
        Swapchain->Release();
    }

    void DX11GraphicsDevice::DX11Texture2D::Release()
    {
        Texture->Release();
        SRV->Release();
        Sampler->Release();
    }


    DX11GraphicsDevice::DX11GraphicsDevice()
        : m_factory(nullptr)
        , m_device(nullptr)
        , m_deviceContext(nullptr)
#if COP_ENABLE_GRAPHICS_API_DEBUG
        , m_dxgiInfoQueue(nullptr)
#endif // COP_ENABLE_GRAPHICS_API_DEBUG
        , m_cbPerCamera(nullptr)
        , m_cbPerMesh(nullptr)
        , m_renderTextureSampler(nullptr)
    {
        _CreateDevice();
#if COP_ENABLE_GRAPHICS_API_DEBUG
        _DebugLayer_Init();
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

        //- Create ConstantBuffers
        {
            D3D11_BUFFER_DESC d3dPerCameraBufferDesc = {
                .ByteWidth           = sizeof(PerCamera),
                .Usage               = D3D11_USAGE_DYNAMIC,
                .BindFlags           = D3D11_BIND_CONSTANT_BUFFER,
                .CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE,
                .MiscFlags           = 0,
                .StructureByteStride = 0,
            };

            D3D11_BUFFER_DESC d3dPerMeshBufferDesc = {
                .ByteWidth           = sizeof(Float4x4),
                .Usage               = D3D11_USAGE_DYNAMIC,
                .BindFlags           = D3D11_BIND_CONSTANT_BUFFER,
                .CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE,
                .MiscFlags           = 0,
                .StructureByteStride = 0,
            };

            m_device->CreateBuffer(&d3dPerCameraBufferDesc, nullptr, &m_cbPerCamera);
            m_device->CreateBuffer(&d3dPerMeshBufferDesc, nullptr, &m_cbPerMesh);
        }

        //- Create RenderTexture sampler
        {
            D3D11_SAMPLER_DESC d3dSamplerDesc = {
                .Filter         = D3D11_FILTER_MIN_MAG_MIP_POINT,
                .AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP,
                .AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP,
                .AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP,
                .MipLODBias     = 0,
                .MaxAnisotropy  = 0,
                .ComparisonFunc = D3D11_COMPARISON_NEVER,
                // .BorderColor      // NOTE(v.matushkin): Only used if D3D11_TEXTURE_ADDRESS_BORDER is specified in Address*
                .MinLOD         = 0,
                .MaxLOD         = D3D11_FLOAT32_MAX,
            };
            m_device->CreateSamplerState(&d3dSamplerDesc, &m_renderTextureSampler);
        }

        //- Set default state
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_deviceContext->GSSetShader(nullptr, nullptr, 0);
        m_deviceContext->HSSetShader(nullptr, nullptr, 0);
        m_deviceContext->DSSetShader(nullptr, nullptr, 0);
        m_deviceContext->CSSetShader(nullptr, nullptr, 0);
    }

    DX11GraphicsDevice::~DX11GraphicsDevice()
    {
        //- GraphicsBuffers
        COP_LOG_WARN_COND(m_graphicsBuffers.size() != 0, "{:d} GraphicsBuffer(s) were not cleaned up before DX11GraphicsDevice destruction", m_meshes.size());
        for (auto& handleAndGraphicsBuffer : m_graphicsBuffers)
        {
            handleAndGraphicsBuffer.second.Release();
        }
        //- Meshes
        COP_LOG_WARN_COND(m_meshes.size() != 0, "{:d} Mesh(es) were not cleaned up before DX11GraphicsDevice destruction", m_meshes.size());
        for (auto& handleAndMesh : m_meshes)
        {
            handleAndMesh.second.Release();
        }
        //- RenderPasses
        COP_LOG_WARN_COND(m_renderPasses.size() != 0, "{:d} RenderPass(es) were not cleaned up before DX11GraphicsDevice destruction", m_renderPasses.size());
        //- Swapchains (Should be destroyed before RenderTextures)
        COP_LOG_WARN_COND(m_swapchains.size() != 0, "{:d} Swapchain(s) were not cleaned up before DX11GraphicsDevice destruction", m_swapchains.size());
        for (auto& handleAndSwapchain : m_swapchains)
        {
            handleAndSwapchain.second.Release(this);
        }
        //- RenderTextures
        COP_LOG_WARN_COND(m_renderTextures.size() != 0, "{:d} RenderTexture(s) were not cleaned up before DX11GraphicsDevice destruction", m_renderTextures.size());
        for (auto& handleAndRenderTexture : m_renderTextures)
        {
            handleAndRenderTexture.second.Release();
        }
        //- Shaders
        COP_LOG_WARN_COND(m_shaders.size() != 0, "{:d} Shader(s) were not cleaned up before DX11GraphicsDevice destruction", m_shaders.size());
        for (auto& handleAndShader : m_shaders)
        {
            handleAndShader.second.Release();
        }
        //- Textures
        COP_LOG_WARN_COND(m_textures.size() != 0, "{:d} Texture(s) were not cleaned up before DX11GraphicsDevice destruction", m_textures.size());
        for (auto& handleAndTexture2D : m_textures)
        {
            handleAndTexture2D.second.Release();
        }

        RELEASE_COM_PTR(m_renderTextureSampler);

        RELEASE_COM_PTR(m_cbPerCamera);
        RELEASE_COM_PTR(m_cbPerMesh);

        // NOTE(v.matushkin): https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-flush
        // NOTE(v.matushkin): Probably this should be called for debug only
        m_deviceContext->ClearState();
        m_deviceContext->Flush();
        RELEASE_COM_PTR(m_deviceContext);

#if COP_ENABLE_GRAPHICS_API_DEBUG
        _DebugLayer_ReportLiveObjects();
        RELEASE_COM_PTR(m_dxgiInfoQueue);
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

        RELEASE_COM_PTR(m_device);
        RELEASE_COM_PTR(m_factory);
    }


    void* DX11GraphicsDevice::GetNativeRenderTexture(RenderTextureHandle renderTextureHandle)
    {
        ID3D11ShaderResourceView* d3dRenderTextureSrv = m_renderTextures.find(renderTextureHandle)->second.SRV;
        COP_ASSERT_MSG(d3dRenderTextureSrv != nullptr, "Trying to access nullptr DX11RenderTexture.SRV");
        return reinterpret_cast<void*>(d3dRenderTextureSrv);
    }

    RenderTextureHandle DX11GraphicsDevice::GetSwapchainRenderTexture(SwapchainHandle swapchainHandle)
    {
        const auto dx11SwapchainIterator = m_swapchains.find(swapchainHandle);
        COP_ASSERT(dx11SwapchainIterator != m_swapchains.end());

        return dx11SwapchainIterator->second.SwapchainRTHandle;
    }


    void DX11GraphicsDevice::BeginFrame(const Float4x4& objectToWorld, const Float4x4& cameraView, const Float4x4& cameraProjection)
    {
        //- Update constant buffers
        {
            PerCamera perCamera = {
                ._CameraView       = cameraView,
                ._CameraProjection = cameraProjection,
            };

            D3D11_MAPPED_SUBRESOURCE d3dMappedSubresource = {
                .pData      = nullptr,
                .RowPitch   = 0,
                .DepthPitch = 0,
            };

            //-- PerCamera
            m_deviceContext->Map(m_cbPerCamera, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedSubresource);
            std::memcpy(d3dMappedSubresource.pData, &perCamera, sizeof(PerCamera));
            m_deviceContext->Unmap(m_cbPerCamera, 0);

            //-- PerMesh
            m_deviceContext->Map(m_cbPerMesh, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedSubresource);
            std::memcpy(d3dMappedSubresource.pData, &objectToWorld, sizeof(Float4x4));
            m_deviceContext->Unmap(m_cbPerMesh, 0);
        }

        //- Set constant buffers
        {
            ID3D11Buffer* constantBuffers[]{ m_cbPerCamera, m_cbPerMesh };
            m_deviceContext->VSSetConstantBuffers(0, 2, constantBuffers); // NOTE(v.matushkin): VSSetConstantBuffers1 ?
        }
    }

    void DX11GraphicsDevice::BeginRenderPass(RenderPassHandle renderPassHandle)
    {
        const auto dx11RenderPassIterator = m_renderPasses.find(renderPassHandle);
        COP_ASSERT(dx11RenderPassIterator != m_renderPasses.end());

        const DX11RenderPass& dx11RenderPass = dx11RenderPassIterator->second;

        //- Clear RenderTargets
        //-- Color
        for (uint8 i = 0; i < dx11RenderPass.NumClearColorAttachments; i++)
        {
            const DX11RenderTexture& dx11RenderTexture = m_renderTextures.find(dx11RenderPass.ClearColorAttachments[i])->second;

            m_deviceContext->ClearRenderTargetView(dx11RenderTexture.RTV, dx11RenderTexture.ClearValue.Color.Value);
        }
        //-- DepthStencil
        if (dx11RenderPass.ClearDepthStencilAttachment != RenderTextureHandle::Invalid)
        {
            const DX11RenderTexture& dx11RenderTexture = m_renderTextures.find(dx11RenderPass.ClearDepthStencilAttachment)->second;
            const uint32 depthStencilClearFlags = dx11_RenderTextureTypeToClearFlags(dx11RenderTexture.Type);
            const ClearDepthStencilValue clearDepthStencilValue = dx11RenderTexture.ClearValue.DepthStencil;

            m_deviceContext->ClearDepthStencilView(
                dx11RenderTexture.DSV,
                depthStencilClearFlags,
                clearDepthStencilValue.Depth,
                clearDepthStencilValue.Stencil
            );
        }

        //- Set RenderTargets
        {
            ID3D11RenderTargetView* d3dSubpassRTVs[8]; // Actual type -> ID3D11RenderTargetView1
            ID3D11DepthStencilView* d3dSubpassDSV;

            for (uint8 i = 0; i < dx11RenderPass.Subpass.NumColorAttachments; i++)
            {
                d3dSubpassRTVs[i] = m_renderTextures.find(dx11RenderPass.Subpass.ColorAttachments[i])->second.RTV;
            }

            if (dx11RenderPass.Subpass.DepthStencilAttachment != RenderTextureHandle::Invalid)
            {
                d3dSubpassDSV = m_renderTextures.find(dx11RenderPass.Subpass.DepthStencilAttachment)->second.DSV;
            }
            else
            {
                d3dSubpassDSV = nullptr;
            }

            m_deviceContext->OMSetRenderTargets(dx11RenderPass.Subpass.NumColorAttachments, d3dSubpassRTVs, d3dSubpassDSV);
        }
    }

    void DX11GraphicsDevice::EndFrame()
    {
        // NOTE(v.matushkin): To disable debug layer message:
        //  'ID3D11DeviceContext::OMSetRenderTargets: Forcing OM Render Target slot 0 to NULL , since calling Present for DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
        //   SwapChains unbinds backbuffer 0 from all GPU writeable bind points, except for DXGI_PRESENT_DO_NOT_SEQUENCE or DXGI_PRESENT_TEST usage.'
        //  Not sure that I need to call this and that this is the right way
        m_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);

        const DXGI_PRESENT_PARAMETERS dxgiPresentParameters = {};
        for (const auto& handleAndSwapchain : m_swapchains)
        {
            // TODO(v.matushkin): VSync settings
            handleAndSwapchain.second.Swapchain->Present1(0, 0, &dxgiPresentParameters);
        }

        COP_DX11_LOG_DEBUG_LAYER();
    }


    void DX11GraphicsDevice::SetViewport(const Rect& viewportRect)
    {
        const D3D11_VIEWPORT d3dViewport = {
            .TopLeftX = viewportRect.X,
            .TopLeftY = viewportRect.Y,
            .Width    = viewportRect.Width,
            .Height   = viewportRect.Height,
            .MinDepth = k_ViewportMinDepth,
            .MaxDepth = k_ViewportMaxDepth,
        };
        m_deviceContext->RSSetViewports(1, &d3dViewport);
    }

    void DX11GraphicsDevice::SetScissorRect(const RectInt& scissorRect)
    {
        const D3D11_RECT d3dScissorRect = {
            .left   = scissorRect.X,
            .top    = scissorRect.Y,
            .right  = scissorRect.GetMaxX(),
            .bottom = scissorRect.GetMaxY(),
        };
        m_deviceContext->RSSetScissorRects(1, &d3dScissorRect);
    }


    void DX11GraphicsDevice::BindShader(ShaderHandle shaderHandle)
    {
        const auto dx11ShaderIterator = m_shaders.find(shaderHandle);
        COP_ASSERT(dx11ShaderIterator != m_shaders.end());

        const DX11Shader& dx11Shader = dx11ShaderIterator->second;

        m_deviceContext->IASetInputLayout(dx11Shader.InputLayout);
        m_deviceContext->VSSetShader(dx11Shader.VertexShader, nullptr, 0);
        m_deviceContext->RSSetState(dx11Shader.RasterizerState);
        m_deviceContext->PSSetShader(dx11Shader.PixelShader, nullptr, 0);
        // TODO(v.matushkin): StencilRef ?
        m_deviceContext->OMSetDepthStencilState(dx11Shader.DepthStencilState, D3D11_DEFAULT_STENCIL_REFERENCE);
        // TODO(v.matushkin): BlendFactor, SampleMask ? BlendFactor = nullptr -> {1,1,1,1}
        m_deviceContext->OMSetBlendState(dx11Shader.BlendState, nullptr, D3D11_DEFAULT_SAMPLE_MASK);
    }

    void DX11GraphicsDevice::BindVertexBuffer(GraphicsBufferHandle vertexBufferHandle, uint32 stride, uint32 offset)
    {
        const auto dx11VertexBufferIterator = m_graphicsBuffers.find(vertexBufferHandle);
        COP_ASSERT(dx11VertexBufferIterator != m_graphicsBuffers.end());

        m_deviceContext->IASetVertexBuffers(0, 1, &dx11VertexBufferIterator->second.Buffer, &stride, &offset);
    }

    void DX11GraphicsDevice::BindIndexBuffer(GraphicsBufferHandle indexBufferHandle, IndexFormat indexFormat)
    {
        const auto dx11IndexBufferIterator = m_graphicsBuffers.find(indexBufferHandle);
        COP_ASSERT(dx11IndexBufferIterator != m_graphicsBuffers.end());

        m_deviceContext->IASetIndexBuffer(dx11IndexBufferIterator->second.Buffer, dx11_IndexFormat(indexFormat), 0);
    }

    void DX11GraphicsDevice::BindConstantBuffer(GraphicsBufferHandle constantBufferHandle, uint32 slot)
    {
        const auto dx11ConstantBufferIterator = m_graphicsBuffers.find(constantBufferHandle);
        COP_ASSERT(dx11ConstantBufferIterator != m_graphicsBuffers.end());

        m_deviceContext->VSSetConstantBuffers(slot, 1, &dx11ConstantBufferIterator->second.Buffer);
    }

    void DX11GraphicsDevice::BindTexture(TextureHandle textureHandle, uint32 slot)
    {
        const auto dx11TextureIterator = m_textures.find(textureHandle);
        COP_ASSERT(dx11TextureIterator != m_textures.end());

        DX11Texture2D& dx11Texture = dx11TextureIterator->second;
        ID3D11ShaderResourceView* d3dTextureSRV = dx11Texture.SRV;
        m_deviceContext->PSSetShaderResources(slot, 1, &d3dTextureSRV);
        m_deviceContext->PSSetSamplers(slot, 1, &dx11Texture.Sampler);
    }

    void DX11GraphicsDevice::BindTexture(RenderTextureHandle renderTextureHandle, uint32 slot)
    {
        const auto dx11RenderTextureIterator = m_renderTextures.find(renderTextureHandle);
        COP_ASSERT(dx11RenderTextureIterator != m_renderTextures.end());

        DX11RenderTexture& dx11RenderTexture = dx11RenderTextureIterator->second;
        ID3D11ShaderResourceView* d3dRenderTextureSRV = dx11RenderTexture.SRV;
        COP_ASSERT_MSG(d3dRenderTextureSRV != nullptr, "Trying to access nullptr DX11RenderTexture.SRV");

        m_deviceContext->PSSetShaderResources(slot, 1, &d3dRenderTextureSRV);
        m_deviceContext->PSSetSamplers(slot, 1, &m_renderTextureSampler);
    }

    void DX11GraphicsDevice::BindMaterial(TextureHandle baseColorTextureHandle, TextureHandle normalTextureHandle)
    {
        const auto dx11BaseColorTextureIterator = m_textures.find(baseColorTextureHandle);
        const auto dx11NormalTextureIterator = m_textures.find(normalTextureHandle);
        COP_ASSERT(dx11BaseColorTextureIterator != m_textures.end());
        COP_ASSERT(dx11NormalTextureIterator != m_textures.end());

        DX11Texture2D& dx11BaseColorTexture = dx11BaseColorTextureIterator->second;
        DX11Texture2D& dx11NormalTexture = dx11NormalTextureIterator->second;

        ID3D11ShaderResourceView* materialTextures[] = { dx11BaseColorTexture.SRV, dx11NormalTexture.SRV };
        ID3D11SamplerState* textureSamplers[] = { dx11BaseColorTexture.Sampler, dx11NormalTexture.Sampler };

        m_deviceContext->PSSetShaderResources(0, 2, materialTextures);
        m_deviceContext->PSSetSamplers(0, 2, textureSamplers);
    }


    void DX11GraphicsDevice::DrawIndexed(uint32 indexCount, uint32 firstIndex, uint32 vertexOffset)
    {
        m_deviceContext->DrawIndexed(indexCount, firstIndex, vertexOffset);
    }

    void DX11GraphicsDevice::DrawMesh(MeshHandle meshHandle)
    {
        const auto dx11MeshIterator = m_meshes.find(meshHandle);
        COP_ASSERT(dx11MeshIterator != m_meshes.end());

        const DX11Mesh& dx11Mesh = dx11MeshIterator->second;

        m_deviceContext->IASetIndexBuffer(dx11Mesh.Index, dx11Mesh.IndexFormat, 0);
        m_deviceContext->IASetVertexBuffers(0, 3, dx11Mesh.VertexBuffers, dx11Mesh.VertexStrides, dx11Mesh.VertexOffsets);

        m_deviceContext->DrawIndexed(dx11Mesh.IndexCount, 0, 0);
    }

    void DX11GraphicsDevice::DrawProcedural(uint32 vertexCount)
    {
        m_deviceContext->Draw(vertexCount, 0);
    }


    GraphicsBufferHandle DX11GraphicsDevice::CreateGraphicsBuffer(const GraphicsBufferDesc& graphicsBufferDesc, std::span<const uint8> initialData)
    {
        DX11GraphicsBuffer dx11GraphicsBuffer;

        {
            // NOTE(v.matushkin): Hardcoding D3D11_USAGE_DYNAMIC and D3D11_CPU_ACCESS_WRITE
            //  as there is no way to tell if the buffer will be updated later or it will be IMMUTABLE
            D3D11_BUFFER_DESC d3dBufferDesc = {
                .ByteWidth           = static_cast<uint32>(graphicsBufferDesc.SizeInBytes()),
                .Usage               = D3D11_USAGE_DYNAMIC,
                .BindFlags           = dx11_GraphicsBufferBindFlags(graphicsBufferDesc.Usage),
                .CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE,
                .MiscFlags           = 0,
                .StructureByteStride = 0,
            };

            D3D11_SUBRESOURCE_DATA* d3dSubresourceDataPtr;
            D3D11_SUBRESOURCE_DATA d3dSubresourceData;

            if (initialData.empty())
            {
                d3dSubresourceDataPtr = nullptr;
            }
            else
            {
                d3dSubresourceData.pSysMem = initialData.data();
                d3dSubresourceData.SysMemPitch = 0;
                d3dSubresourceData.SysMemSlicePitch = 0;

                d3dSubresourceDataPtr = &d3dSubresourceData;
            }

            m_device->CreateBuffer(&d3dBufferDesc, d3dSubresourceDataPtr, &dx11GraphicsBuffer.Buffer);
        }

        const auto graphicsBufferHandle = static_cast<GraphicsBufferHandle>(g_GraphicsBufferHandleWorkaround++);
        m_graphicsBuffers.emplace(graphicsBufferHandle, dx11GraphicsBuffer);

        return graphicsBufferHandle;
    }

    MeshHandle DX11GraphicsDevice::CreateMesh(const MeshDesc& meshDesc)
    {
        DX11Mesh dx11Mesh;
        dx11Mesh.IndexFormat = dx11_IndexFormat(meshDesc.IndexFormat);
        dx11Mesh.IndexCount = meshDesc.IndexCount;
        dx11Mesh.VertexStrides[0] = meshDesc.Position.Stride();
        dx11Mesh.VertexStrides[1] = meshDesc.Normal.Stride();
        dx11Mesh.VertexStrides[2] = meshDesc.UV0.Stride();
        dx11Mesh.VertexOffsets[0] = meshDesc.Position.Offset;
        dx11Mesh.VertexOffsets[1] = meshDesc.Normal.Offset;
        dx11Mesh.VertexOffsets[2] = meshDesc.UV0.Offset;

        //- Create Index Buffer
        {
            D3D11_BUFFER_DESC d3dIndexBufferDesc = {
                .ByteWidth           = static_cast<uint32>(meshDesc.IndexData.size()),
                .Usage               = D3D11_USAGE_IMMUTABLE,
                .BindFlags           = D3D11_BIND_INDEX_BUFFER,
                .CPUAccessFlags      = 0,
                .MiscFlags           = 0,
                .StructureByteStride = 0,
            };
            D3D11_SUBRESOURCE_DATA d3dIndexSubresourceData = {
                .pSysMem          = meshDesc.IndexData.data(),
                .SysMemPitch      = 0,
                .SysMemSlicePitch = 0,
            };
            m_device->CreateBuffer(&d3dIndexBufferDesc, &d3dIndexSubresourceData, &dx11Mesh.Index);
        }

        //- Create Vertex Buffers
        const uint8* vertexData = meshDesc.VertexData.data();
        const uint32 vertexCount = meshDesc.VertexCount;
        uint32 vertexDataOffset = 0;

        // NOTE(v.matushkin): Use lambda?
        //-- Position
        {
            const uint32 vertexAttributeDataSize = vertexCount * dx11Mesh.VertexStrides[0];

            D3D11_BUFFER_DESC d3dVertexAttributeBufferDesc = {
                .ByteWidth           = vertexAttributeDataSize,
                .Usage               = D3D11_USAGE_IMMUTABLE,
                .BindFlags           = D3D11_BIND_VERTEX_BUFFER,
                .CPUAccessFlags      = 0,
                .MiscFlags           = 0,
                .StructureByteStride = 0,
            };
            D3D11_SUBRESOURCE_DATA d3dVertexAttributeSubresourceData = {
                .pSysMem          = vertexData,
                .SysMemPitch      = 0,
                .SysMemSlicePitch = 0,
            };
            m_device->CreateBuffer(&d3dVertexAttributeBufferDesc, &d3dVertexAttributeSubresourceData, &dx11Mesh.VertexBuffers[0]);

            vertexDataOffset += vertexAttributeDataSize;
        }
        //-- Normal
        {
            const uint32 vertexAttributeDataSize = vertexCount * dx11Mesh.VertexStrides[1];

            D3D11_BUFFER_DESC d3dVertexAttributeBufferDesc = {
                .ByteWidth           = vertexAttributeDataSize,
                .Usage               = D3D11_USAGE_IMMUTABLE,
                .BindFlags           = D3D11_BIND_VERTEX_BUFFER,
                .CPUAccessFlags      = 0,
                .MiscFlags           = 0,
                .StructureByteStride = 0,
            };
            D3D11_SUBRESOURCE_DATA d3dVertexAttributeSubresourceData = {
                .pSysMem          = vertexData + vertexDataOffset,
                .SysMemPitch      = 0,
                .SysMemSlicePitch = 0,
            };
            m_device->CreateBuffer(&d3dVertexAttributeBufferDesc, &d3dVertexAttributeSubresourceData, &dx11Mesh.VertexBuffers[1]);

            vertexDataOffset += vertexAttributeDataSize;
        }
        //-- UV0
        {
            const uint32 vertexAttributeDataSize = vertexCount * dx11Mesh.VertexStrides[2];

            D3D11_BUFFER_DESC d3dVertexAttributeBufferDesc = {
                .ByteWidth           = vertexAttributeDataSize,
                .Usage               = D3D11_USAGE_IMMUTABLE,
                .BindFlags           = D3D11_BIND_VERTEX_BUFFER,
                .CPUAccessFlags      = 0,
                .MiscFlags           = 0,
                .StructureByteStride = 0,
            };
            D3D11_SUBRESOURCE_DATA d3dVertexAttributeSubresourceData = {
                .pSysMem          = vertexData + vertexDataOffset,
                .SysMemPitch      = 0,
                .SysMemSlicePitch = 0,
            };
            m_device->CreateBuffer(&d3dVertexAttributeBufferDesc, &d3dVertexAttributeSubresourceData, &dx11Mesh.VertexBuffers[2]);
        }

        const auto meshHandle = static_cast<MeshHandle>(g_MeshHandleWorkaround++);
        m_meshes.emplace(meshHandle, dx11Mesh);

        return meshHandle;
    }

    RenderPassHandle DX11GraphicsDevice::CreateRenderPass(const RenderPassDesc& renderPassDesc)
    {
        DX11RenderPass dx11RenderPass;
        dx11RenderPass.ClearDepthStencilAttachment = RenderTextureHandle::Invalid;
        dx11RenderPass.NumClearColorAttachments = 0;

        RenderTextureHandle depthStencilAttachment;

        //- Clear attachments
        //-- Color
        for (const AttachmentDesc& colorAttachmentDesc : renderPassDesc.ColorAttachments)
        {
            if (colorAttachmentDesc.LoadAction == AttachmentLoadAction::Clear)
            {
                dx11RenderPass.ClearColorAttachments[dx11RenderPass.NumClearColorAttachments++] = colorAttachmentDesc.RTHandle;
            }
        }
        for (uint8 i = dx11RenderPass.NumClearColorAttachments; i < k_RenderPassColorAttachments; i++)
        {
            dx11RenderPass.ClearColorAttachments[i] = RenderTextureHandle::Invalid;
        }
        //-- DepthStencil
        if (renderPassDesc.DepthStencilAttachment.has_value())
        {
            const AttachmentDesc& depthStencilAttachmentDesc = *renderPassDesc.DepthStencilAttachment;
            depthStencilAttachment = depthStencilAttachmentDesc.RTHandle;

            if (depthStencilAttachmentDesc.LoadAction == AttachmentLoadAction::Clear)
            {
                dx11RenderPass.ClearDepthStencilAttachment = depthStencilAttachment;
            }
        }
        else
        {
            depthStencilAttachment = RenderTextureHandle::Invalid;
        }

        //- Subpass Attachments
        //-- Color
        dx11RenderPass.Subpass.NumColorAttachments = static_cast<uint8>(renderPassDesc.Subpass.ColorAttachmentIndices.size());
        for (uint8 i = 0; i < dx11RenderPass.Subpass.NumColorAttachments; i++)
        {
            dx11RenderPass.Subpass.ColorAttachments[i] = renderPassDesc.ColorAttachments[renderPassDesc.Subpass.ColorAttachmentIndices[i]].RTHandle;
        }
        //-- DepthStencil
        if (renderPassDesc.Subpass.UseDepthStencilAttachment)
        {
            COP_ASSERT_MSG(depthStencilAttachment != RenderTextureHandle::Invalid, "Trying to use DepthStencilAttachment in Subpass while there is no DepthStencil RenderTexture in RenderPass");

            dx11RenderPass.Subpass.DepthStencilAttachment = depthStencilAttachment;
        }
        else
        {
            dx11RenderPass.Subpass.DepthStencilAttachment = RenderTextureHandle::Invalid;
        }

        const auto renderPassHandle = static_cast<RenderPassHandle>(g_RenderPassHandleWorkaround++);
        m_renderPasses.emplace(renderPassHandle, dx11RenderPass);

        return renderPassHandle;
    }

    RenderTextureHandle DX11GraphicsDevice::CreateRenderTexture(const RenderTextureDesc& renderTextureDesc)
    {
        DX11RenderTexture dx11RenderTexture;
        dx11RenderTexture.ClearValue = renderTextureDesc.ClearValue;
        dx11RenderTexture.Type = renderTextureDesc.RenderTextureType();

        const DXGI_FORMAT dxgiRenderTextureFormat = dx_RenderTextureFormat(renderTextureDesc.Format);
        const bool isUsageShaderRead = renderTextureDesc.Usage == RenderTextureUsage::ShaderRead;
        const bool isColorRenderTexture = dx11RenderTexture.Type == RenderTextureType::Color;

        //- Create RenderTexture
        {
            uint32 d3dTextureBindFlags = isColorRenderTexture ? D3D11_BIND_RENDER_TARGET : D3D11_BIND_DEPTH_STENCIL;
            if (isUsageShaderRead)
            {
                d3dTextureBindFlags |= D3D11_BIND_SHADER_RESOURCE;
            }
            D3D11_TEXTURE2D_DESC1 d3dTextureDesc = {
                .Width          = renderTextureDesc.Width,
                .Height         = renderTextureDesc.Height,
                .MipLevels      = 1,
                .ArraySize      = 1,
                .Format         = dxgiRenderTextureFormat,
                .SampleDesc     = { .Count = 1, .Quality = 0 },
                .Usage          = D3D11_USAGE_DEFAULT,
                .BindFlags      = d3dTextureBindFlags,
                .CPUAccessFlags = 0,
                .MiscFlags      = 0,                              // NOTE(v.matushkin): Whats this?
                .TextureLayout  = D3D11_TEXTURE_LAYOUT_UNDEFINED, // Can use only UNDEFINED if CPUAccessFlags = 0
            };
            m_device->CreateTexture2D1(&d3dTextureDesc, nullptr, &dx11RenderTexture.Texture);
        }

        //- Create RTV or DSV
        if (isColorRenderTexture)
        {
            dx11RenderTexture.DSV = nullptr;

            D3D11_RENDER_TARGET_VIEW_DESC1 d3dRtvDesc = {
                .Format        = dxgiRenderTextureFormat,
                .ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D,
                .Texture2D     = { .MipSlice = 0 },
            };
            m_device->CreateRenderTargetView1(dx11RenderTexture.Texture, &d3dRtvDesc, &dx11RenderTexture.RTV);
        }
        else
        {
            dx11RenderTexture.RTV = nullptr;

            D3D11_DEPTH_STENCIL_VIEW_DESC d3dDsvDesc = {
                .Format        = dxgiRenderTextureFormat,
                .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
                .Flags         = 0, // NOTE(v.matushkin): D3D11_DSV_READ_ONLY_* ?
                .Texture2D     = { .MipSlice = 0 },
            };
            m_device->CreateDepthStencilView(dx11RenderTexture.Texture, &d3dDsvDesc, &dx11RenderTexture.DSV);
        }

        //- Create SRV
        if (isUsageShaderRead)
        {
            D3D11_TEX2D_SRV1 d3dSrvTex2D = {
                .MostDetailedMip = 0,
                .MipLevels       = 1,
                .PlaneSlice      = 0,
            };
            D3D11_SHADER_RESOURCE_VIEW_DESC1 d3dSrvDesc = {
                .Format        = dxgiRenderTextureFormat,
                .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
                .Texture2D     = d3dSrvTex2D,
            };
            m_device->CreateShaderResourceView1(dx11RenderTexture.Texture, &d3dSrvDesc, &dx11RenderTexture.SRV);
        }
        else
        {
            dx11RenderTexture.SRV = nullptr;
        }

        const auto renderTextureHandle = static_cast<RenderTextureHandle>(g_RenderTextureHandleWorkaround++);
        m_renderTextures.emplace(renderTextureHandle, dx11RenderTexture);

        return renderTextureHandle;
    }

    ShaderHandle DX11GraphicsDevice::CreateShader(const ShaderDesc& shaderDesc)
    {
        // https://github-wiki-see.page/m/Microsoft/DirectXTK/wiki/CommonStates

        // NOTE(v.matushkin): Needed to enable Scissors for ImGui shader,
        //  don't know how else I should handle this. There is no point in exposing this flag since
        //  it's always enabled in DX12/Vulkan anyway? May be just enable scissors for every shader?
        // NOTE(v.matushkin): FrontCounterClockwise and InputLayout also depends on it.
        static constinit const std::string_view k_ImGuiShaderName = "Engine/ImGui";
        const bool isImGuiShader = shaderDesc.Name == k_ImGuiShaderName;

        // NOTE(v.matushkin): Can Shader State checks for nullptr fail because I'm not initializing them?
        DX11Shader dx11Shader;

        //- Create RasterizerState
        {
            const RasterizerStateDesc rasterizerStateDesc = shaderDesc.RasterizerStateDesc;

            D3D11_RASTERIZER_DESC2 d3dRasterizerStateDesc = {
                .FillMode              = dx11_PolygonMode(rasterizerStateDesc.PolygonMode),
                .CullMode              = dx11_CullMode(rasterizerStateDesc.CullMode),
                .FrontCounterClockwise = isImGuiShader ? false : dx_TriangleFrontFace(rasterizerStateDesc.FrontFace),
                .DepthBias             = D3D11_DEFAULT_DEPTH_BIAS,
                .DepthBiasClamp        = D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
                .SlopeScaledDepthBias  = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
                .DepthClipEnable       = true,
                .ScissorEnable         = isImGuiShader,
                .MultisampleEnable     = false,
                .AntialiasedLineEnable = false,
                .ForcedSampleCount     = 0,
                .ConservativeRaster    = D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF,
            };
            m_device->CreateRasterizerState2(&d3dRasterizerStateDesc, &dx11Shader.RasterizerState);

            COP_LOG_ERROR_COND(dx11Shader.RasterizerState == nullptr, "Failed to create ID3D11RasterizerState2 for '{}' shader", shaderDesc.Name);
        }
        //- Create DepthStencilState
        {
            const DepthStencilStateDesc depthStencilStateDesc = shaderDesc.DepthStencilStateDesc;

            D3D11_DEPTH_STENCILOP_DESC d3dDepthStencilOpDesc = {
                .StencilFailOp      = D3D11_STENCIL_OP_KEEP,
                .StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
                .StencilPassOp      = D3D11_STENCIL_OP_KEEP,
                .StencilFunc        = D3D11_COMPARISON_ALWAYS,
            };
            D3D11_DEPTH_STENCIL_DESC d3dDepthStencilStateDesc = {
                .DepthEnable      = depthStencilStateDesc.DepthTestEnable,
                .DepthWriteMask   = depthStencilStateDesc.DepthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO,
                .DepthFunc        = dx11_CompareFunction(depthStencilStateDesc.DepthCompareFunction),
                .StencilEnable    = false,
                .StencilReadMask  = 0,                          // Default is 'D3D11_DEFAULT_STENCIL_READ_MASK'
                .StencilWriteMask = 0,                          // Default is 'D3D11_DEFAULT_STENCIL_WRITE_MASK'
                .FrontFace        = d3dDepthStencilOpDesc,
                .BackFace         = d3dDepthStencilOpDesc,
            };
            m_device->CreateDepthStencilState(&d3dDepthStencilStateDesc, &dx11Shader.DepthStencilState);

            COP_LOG_ERROR_COND(dx11Shader.DepthStencilState == nullptr, "Failed to create ID3D11DepthStencilState for '{}' shader", shaderDesc.Name);
        }
        //- Create BlendState
        {
            const BlendStateDesc blendStateDesc = shaderDesc.BlendStateDesc;

            D3D11_RENDER_TARGET_BLEND_DESC1 d3dRenderTargetBlendDesc = {
                // NOTE(v.matushkin): May be there is no need to set this if BlendMode::Off ?
                .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
            };

            if (blendStateDesc.BlendMode == BlendMode::BlendOp)
            {
                d3dRenderTargetBlendDesc.BlendEnable    = true;
                d3dRenderTargetBlendDesc.SrcBlend       = dx11_BlendFactor(blendStateDesc.ColorSrcBlendFactor);
                d3dRenderTargetBlendDesc.DestBlend      = dx11_BlendFactor(blendStateDesc.ColorDstBlendFactor);
                d3dRenderTargetBlendDesc.BlendOp        = dx11_BlendOp(blendStateDesc.ColorBlendOp);
                d3dRenderTargetBlendDesc.SrcBlendAlpha  = dx11_BlendFactor(blendStateDesc.AlphaSrcBlendFactor);
                d3dRenderTargetBlendDesc.DestBlendAlpha = dx11_BlendFactor(blendStateDesc.AlphaDstBlendFactor);
                d3dRenderTargetBlendDesc.BlendOpAlpha   = dx11_BlendOp(blendStateDesc.AlphaBlendOp);
            }
            else if (blendStateDesc.BlendMode == BlendMode::LogicOp)
            {
                d3dRenderTargetBlendDesc.LogicOpEnable = true;
                d3dRenderTargetBlendDesc.LogicOp       = dx11_BlendLogicOp(blendStateDesc.LogicOp);
            }

            D3D11_BLEND_DESC1 d3dBlendStateDesc;
            d3dBlendStateDesc.AlphaToCoverageEnable  = false;
            d3dBlendStateDesc.IndependentBlendEnable = k_IndependentBlendEnable;
            d3dBlendStateDesc.RenderTarget[0]        = d3dRenderTargetBlendDesc;

            m_device->CreateBlendState1(&d3dBlendStateDesc, &dx11Shader.BlendState);

            COP_LOG_ERROR_COND(dx11Shader.BlendState == nullptr, "Failed to create ID3D11BlendState1 for '{}' shader", shaderDesc.Name);
        }

        //- Create InputLayout
        {
            D3D11_INPUT_ELEMENT_DESC d3dInputElementDesc[3];

            if (isImGuiShader)
            {
                d3dInputElementDesc[0] = {
                    .SemanticName         = "POSITION",
                    .SemanticIndex        = 0,
                    .Format               = DXGI_FORMAT_R32G32_FLOAT,
                    .InputSlot            = 0,
                    .AlignedByteOffset    = 0,
                    .InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA,
                    .InstanceDataStepRate = 0,
                };
                d3dInputElementDesc[1] = {
                    .SemanticName         = "TEXCOORD",
                    .SemanticIndex        = 0,
                    .Format               = DXGI_FORMAT_R32G32_FLOAT,
                    .InputSlot            = 0,
                    .AlignedByteOffset    = 8,
                    .InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA,
                    .InstanceDataStepRate = 0,
                };
                d3dInputElementDesc[2] = {
                    .SemanticName         = "COLOR",
                    .SemanticIndex        = 0,
                    .Format               = DXGI_FORMAT_R8G8B8A8_UNORM,
                    .InputSlot            = 0,
                    .AlignedByteOffset    = 16,
                    .InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA,
                    .InstanceDataStepRate = 0,
                };
            }
            else
            {
                d3dInputElementDesc[0] = {
                    .SemanticName         = "POSITION",
                    .SemanticIndex        = 0,
                    .Format               = DXGI_FORMAT_R32G32B32_FLOAT,
                    .InputSlot            = 0,
                    .AlignedByteOffset    = 0,
                    .InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA,
                    .InstanceDataStepRate = 0,
                };
                d3dInputElementDesc[1] = {
                    .SemanticName         = "NORMAL",
                    .SemanticIndex        = 0,
                    .Format               = DXGI_FORMAT_R32G32B32_FLOAT,
                    .InputSlot            = 1,
                    .AlignedByteOffset    = 0,
                    .InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA,
                    .InstanceDataStepRate = 0,
                };
                d3dInputElementDesc[2] = {
                    .SemanticName         = "TEXCOORD",
                    .SemanticIndex        = 0,
                    .Format               = DXGI_FORMAT_R32G32_FLOAT,
                    .InputSlot            = 2,
                    .AlignedByteOffset    = 0,
                    .InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA,
                    .InstanceDataStepRate = 0,
                };
            }

            m_device->CreateInputLayout(d3dInputElementDesc, 3, shaderDesc.VertexBlob.Data.get(), shaderDesc.VertexBlob.Size, &dx11Shader.InputLayout);
        }

        m_device->CreateVertexShader(shaderDesc.VertexBlob.Data.get(), shaderDesc.VertexBlob.Size, nullptr, &dx11Shader.VertexShader);
        m_device->CreatePixelShader(shaderDesc.FragmentBlob.Data.get(), shaderDesc.FragmentBlob.Size, nullptr, &dx11Shader.PixelShader);

        const auto shaderHandle = static_cast<ShaderHandle>(g_ShaderHandleWorkaround++);
        m_shaders.emplace(shaderHandle, dx11Shader);

        return shaderHandle;
    }

    SwapchainHandle DX11GraphicsDevice::CreateSwapchain(const SwapchainDesc& swapchainDesc)
    {
        // TODO(v.matushkin): I guess at least I should use DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH flag?
        DX11Swapchain dx11Swapchain;
        dx11Swapchain.Format = dx_RenderTextureFormat(swapchainDesc.Format);
        dx11Swapchain.Flags = 0;
        dx11Swapchain.BufferCount = swapchainDesc.BufferCount;

        // Dummy Swapchain RenderTexture
        DX11RenderTexture dx11SwapchainRenderTexture;
        dx11SwapchainRenderTexture.DSV = nullptr;
        dx11SwapchainRenderTexture.SRV = nullptr;
        dx11SwapchainRenderTexture.ClearValue = RenderTextureClearValue::DefaultColor();
        dx11SwapchainRenderTexture.Type = RenderTextureType::Color;

        //- Create Swapchain
        {
            DXGI_SAMPLE_DESC dxgiSwapchainSampleDesc = {
                .Count   = 1, // multisampling setting
                .Quality = 0  // vendor-specific flag
            };
            // NOTE(v.matushkin): From bgfx:
            //  According to https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/converting-data-color-space ,
            //   it is OK to create the backbuffer with m_fmt (a non- _SRGB format), and then create the render target view into it
            //   with m_fmtSrgb (the _SRGB format of same), and it will work identically as if you had created the swapchain with
            //   m_fmtSrgb as the backbuffer format.
            //  Moreover, it is actually not desirable to create the backbuffer with an _SRGB format, because that
            //   is incompatible with the flip presentation model, which is desirable for various reasons including player embedding.
            DXGI_SWAP_CHAIN_DESC1 dxgiSwapchainDesc = {
                .Width       = swapchainDesc.Width,
                .Height      = swapchainDesc.Height,
                // TODO(v.matushkin): <SwapchainCreation/Format>
                .Format      = dx11Swapchain.Format,
                .Stereo      = false,
                .SampleDesc  = dxgiSwapchainSampleDesc,
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = swapchainDesc.BufferCount,
                .Scaling     = DXGI_SCALING_STRETCH,            // TODO(v.matushkin): Play with this
                .SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD,
                .AlphaMode   = DXGI_ALPHA_MODE_IGNORE,          // NOTE(v.matushkin): Don't know
                .Flags       = dx11Swapchain.Flags,
            };

            IDXGISwapChain1* dxgiSwapchain1;

            m_factory->CreateSwapChainForHwnd(
                m_device,
                reinterpret_cast<HWND>(swapchainDesc.WindowNativeHandle),
                &dxgiSwapchainDesc,
                nullptr,            // TODO(v.matushkin): Fullscreen swap chain?
                nullptr,            // NOTE(v.matushkin): Useless?
                &dxgiSwapchain1
            );
            dxgiSwapchain1->QueryInterface(&dx11Swapchain.Swapchain);
            dxgiSwapchain1->Release();
        }

        //- Get Swapchain RenderTexture
        {
            dx11Swapchain.Swapchain->GetBuffer(0, IID_PPV_ARGS(&dx11SwapchainRenderTexture.Texture));
            // NOTE(v.matushkin): ViewDesc?
            m_device->CreateRenderTargetView1(dx11SwapchainRenderTexture.Texture, nullptr, &dx11SwapchainRenderTexture.RTV);
        }

        //- Setup Viewport
        {
            EngineSettings& engineSettings = EngineSettings::Get();
            // TODO(v.matushkin): <Viewport>
            D3D11_VIEWPORT d3dViewport = {
                .Width    = static_cast<float32>(engineSettings.RenderWidth),
                .Height   = static_cast<float32>(engineSettings.RenderHeight),
                .MinDepth = 0,
                .MaxDepth = 1,
            };
            m_deviceContext->RSSetViewports(1, &d3dViewport);
        }

        dx11Swapchain.SwapchainRTHandle = static_cast<RenderTextureHandle>(g_RenderTextureHandleWorkaround++);
        m_renderTextures.emplace(dx11Swapchain.SwapchainRTHandle, dx11SwapchainRenderTexture);

        const auto swapchainHandle = static_cast<SwapchainHandle>(g_SwapchainHandleWorkaround++);
        m_swapchains.emplace(swapchainHandle, dx11Swapchain);

        return swapchainHandle;
    }

    TextureHandle DX11GraphicsDevice::CreateTexture2D(const TextureDesc& textureDesc)
    {
        const DXGI_FORMAT dxgiTextureFormat = dx_TextureFormat(textureDesc.Format);

        DX11Texture2D dx11Texture2D;

        //- Create Texture
        {
            // NOTE(v.matushkin): Autogenerated mipmaps (not tested)
            //  - D3D11_TEXTURE2D_DESC1 requirements
            //    - MipLevels = 0
            //    - Usage != D3D11_USAGE_IMMUTABLE
            //    - BindFlags |= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET
            //    - MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS
            //  - device->CreateTexture2D(DESC, nullptr, &ID3DTexture2D)
            //  - context->UpdateSubresource with Mip0
            //  - context->GenerateMips

            DXGI_SAMPLE_DESC      dxgiSampleDesc = { .Count = 1, .Quality = 0 };
            D3D11_TEXTURE2D_DESC1 d3dTextureDesc = {
                .Width          = textureDesc.Width,
                .Height         = textureDesc.Height,
                .MipLevels      = textureDesc.MipmapCount,
                .ArraySize      = 1,
                .Format         = dxgiTextureFormat,
                .SampleDesc     = dxgiSampleDesc,
                .Usage          = D3D11_USAGE_IMMUTABLE,
                .BindFlags      = D3D11_BIND_SHADER_RESOURCE,
                .CPUAccessFlags = 0,
                .MiscFlags      = 0,
                .TextureLayout  = D3D11_TEXTURE_LAYOUT_UNDEFINED, // Can use only UNDEFINED if CPUAccessFlags = 0
            };

            auto d3dTextureSubresources = std::make_unique<D3D11_SUBRESOURCE_DATA[]>(textureDesc.MipmapCount);

            const uint8 bytesPerPixel = TextureFormat_BytesPerPixel(textureDesc.Format);
            const uint8* textureDataPtr = textureDesc.Data.data();
            uint32 mipWidth = textureDesc.Width;
            uint32 mipHeight = textureDesc.Height;

            for (uint8 i = 0; i < textureDesc.MipmapCount; i++)
            {
                const uint32 mipRowBytes = mipWidth * bytesPerPixel;

                d3dTextureSubresources[i] = D3D11_SUBRESOURCE_DATA{
                    .pSysMem          = textureDataPtr,
                    .SysMemPitch      = mipRowBytes,
                    .SysMemSlicePitch = 0,
                };

                textureDataPtr += mipRowBytes * mipHeight;
                mipWidth >>= 1;
                mipHeight >>= 1;

                if (mipWidth == 0) mipWidth = 1;
                if (mipHeight == 0) mipHeight = 1;
            }

            m_device->CreateTexture2D1(&d3dTextureDesc, d3dTextureSubresources.get(), &dx11Texture2D.Texture);
        }
        //- Create Texture SRV
        {
            D3D11_TEX2D_SRV1 d3dSrvTex2D = {
                .MostDetailedMip = 0,
                .MipLevels       = textureDesc.MipmapCount, // NOTE(v.matushkin): Can be set to uint32(-1) to use all mipmaps
                .PlaneSlice      = 0,
            };
            // TODO(v.matushkin): Don't know how to use ID3D11ShaderResourceView1
            //  if m_deviceContext->PSSetShaderResources takes only ID3D11ShaderResourceView
            D3D11_SHADER_RESOURCE_VIEW_DESC1 d3dSrvDesc = {
                .Format        = dxgiTextureFormat,
                .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
                .Texture2D     = d3dSrvTex2D,
            };
            m_device->CreateShaderResourceView1(dx11Texture2D.Texture, &d3dSrvDesc, &dx11Texture2D.SRV);
        }
        //- Create Texture Sampler
        {
            // NOTE(v.matushkin): I don't know if I should "fix" Filter/MaxAnisotropy in some cases,
            //  like if AnisotropicLevel==1 -> set FilterMode to Trilinear,
            //  or if FilterMode != Anisotropic -> set MaxAnisotropy = 0.
            //  Or may be I should do it at the TextureDesc/Texture level and here assume that everything is already set up properly.

            D3D11_SAMPLER_DESC d3dSamplerDesc = {
                .Filter         = dx11_TextureFilterMode(textureDesc.FilterMode),
                .AddressU       = dx11_TextureWrapMode(textureDesc.WrapModeU),
                .AddressV       = dx11_TextureWrapMode(textureDesc.WrapModeV),
                .AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP,  // NOTE(v.matushkin): How to handle this for 2D textures?
                .MipLODBias     = 0,
                .MaxAnisotropy  = textureDesc.AnisotropicLevel,
                .ComparisonFunc = D3D11_COMPARISON_NEVER,
                // .BorderColor      // NOTE(v.matushkin): Only used if D3D11_TEXTURE_ADDRESS_BORDER is specified in Address*
                .MinLOD         = 0,
                .MaxLOD         = D3D11_FLOAT32_MAX,
            };
            m_device->CreateSamplerState(&d3dSamplerDesc, &dx11Texture2D.Sampler);
        }

        const auto textureHandle = static_cast<TextureHandle>(g_TextureHandleWorkaround++);
        m_textures.emplace(textureHandle, dx11Texture2D);

        return textureHandle;
    }


    void DX11GraphicsDevice::UpdateGraphicsBuffer(GraphicsBufferHandle graphicsBufferHandle, std::span<const uint8> data)
    {
        const auto dx11GraphicsBufferIterator = m_graphicsBuffers.find(graphicsBufferHandle);
        COP_ASSERT(dx11GraphicsBufferIterator != m_graphicsBuffers.end());

        ID3D11Buffer* d3dBuffer = dx11GraphicsBufferIterator->second.Buffer;

        {
            D3D11_MAPPED_SUBRESOURCE d3dMappedSubresource = {
                .pData      = nullptr,
                .RowPitch   = 0,
                .DepthPitch = 0,
            };

            m_deviceContext->Map(d3dBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedSubresource);
            std::memcpy(d3dMappedSubresource.pData, data.data(), data.size());
            m_deviceContext->Unmap(d3dBuffer, 0);
        }
    }

    void DX11GraphicsDevice::ResizeSwapchain(SwapchainHandle swapchainHandle, uint16 width, uint16 height)
    {
        const auto dx11SwapchainIterator = m_swapchains.find(swapchainHandle);
        COP_ASSERT(dx11SwapchainIterator != m_swapchains.end());

        DX11Swapchain& dx11Swapchain = dx11SwapchainIterator->second;
        DX11RenderTexture& dx11SwapchainRenderTexture = m_renderTextures.find(dx11Swapchain.SwapchainRTHandle)->second;
        dx11SwapchainRenderTexture.RTV->Release();
        dx11SwapchainRenderTexture.Texture->Release();
        dx11Swapchain.Swapchain->ResizeBuffers(dx11Swapchain.BufferCount, width, height, dx11Swapchain.Format, dx11Swapchain.Flags);

        //- Get Swapchain RenderTexture
        {
            dx11Swapchain.Swapchain->GetBuffer(0, IID_PPV_ARGS(&dx11SwapchainRenderTexture.Texture));
            m_device->CreateRenderTargetView1(dx11SwapchainRenderTexture.Texture, nullptr, &dx11SwapchainRenderTexture.RTV);
        }

        //- Setup Viewport
        {
            EngineSettings& engineSettings = EngineSettings::Get();
            // TODO(v.matushkin): <Viewport>
            D3D11_VIEWPORT d3dViewport = {
                .Width    = static_cast<float32>(engineSettings.RenderWidth),
                .Height   = static_cast<float32>(engineSettings.RenderHeight),
                .MinDepth = 0,
                .MaxDepth = 1,
            };
            m_deviceContext->RSSetViewports(1, &d3dViewport);
        }
    }


    void DX11GraphicsDevice::DestroyGraphicsBuffer(GraphicsBufferHandle graphicsBufferHandle)
    {
        auto graphicsBufferMapNode = m_graphicsBuffers.extract(graphicsBufferHandle);
        COP_ASSERT(graphicsBufferMapNode.empty() == false);

        graphicsBufferMapNode.mapped().Release();
    }

    void DX11GraphicsDevice::DestroyMesh(MeshHandle meshHandle)
    {
        auto meshesMapNode = m_meshes.extract(meshHandle);
        COP_ASSERT(meshesMapNode.empty() == false);

        meshesMapNode.mapped().Release();
    }

    void DX11GraphicsDevice::DestroyRenderPass(RenderPassHandle renderPassHandle)
    {
        auto renderPassesMapNode = m_renderPasses.extract(renderPassHandle);
        COP_ASSERT(renderPassesMapNode.empty() == false);
    }

    void DX11GraphicsDevice::DestroyRenderTexture(RenderTextureHandle renderTextureHandle)
    {
        auto renderTexturesMapNode = m_renderTextures.extract(renderTextureHandle);
        COP_ASSERT(renderTexturesMapNode.empty() == false);

        renderTexturesMapNode.mapped().Release();
    }

    void DX11GraphicsDevice::DestroyShader(ShaderHandle shaderHandle)
    {
        auto shadersMapNode = m_shaders.extract(shaderHandle);
        COP_ASSERT(shadersMapNode.empty() == false);

        shadersMapNode.mapped().Release();
    }

    void DX11GraphicsDevice::DestroySwapchain(SwapchainHandle swapchainHandle)
    {
        auto swapchainsMapNode = m_swapchains.extract(swapchainHandle);
        COP_ASSERT(swapchainsMapNode.empty() == false);

        swapchainsMapNode.mapped().Release(this);
    }

    void DX11GraphicsDevice::DestroyTexture2D(TextureHandle textureHandle)
    {
        auto texturesMapNode = m_textures.extract(textureHandle);
        COP_ASSERT(texturesMapNode.empty() == false);

        texturesMapNode.mapped().Release();
    }


    void DX11GraphicsDevice::_CreateDevice()
    {
        //- Create Factory and Adapter
        IDXGIAdapter3* dxgiAdapter3;
        {
            uint32 dxgiFactoryFlags = 0;
#if COP_ENABLE_GRAPHICS_API_DEBUG
            // NOTE(v.matushkin): Looks like I need to set this to use IDXGIInfoQueue?
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

            IDXGIAdapter1* dxgiAdapter;

            CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory));
            m_factory->EnumAdapters1(0, &dxgiAdapter);
            dxgiAdapter->QueryInterface(&dxgiAdapter3);
            dxgiAdapter->Release();

            //-- Get GPU description
#if COP_ENABLE_LOGGING
            DXGI_ADAPTER_DESC2 dxgiAdapterDesc;
            dxgiAdapter3->GetDesc2(&dxgiAdapterDesc);
            COP_LOG_INFO(L"DirectX11 Adapter: {}", dxgiAdapterDesc.Description);
#endif // COP_ENABLE_LOGGING
        }

        //- Create Device and DeviceContext
        {
            // NOTE(v.matushkin): Do I even need this?
            //  This flag adds support for surfaces with a color-channel ordering different
            //  from the API default. It is required for compatibility with Direct2D.
            uint32 d3dDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if COP_ENABLE_GRAPHICS_API_DEBUG
            d3dDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

            D3D_FEATURE_LEVEL d3dFeatureLevels[] = { D3D_FEATURE_LEVEL_11_1 };
            D3D_FEATURE_LEVEL useless_feature_level_variable;

            ID3D11Device*        d3dDevice;
            ID3D11DeviceContext* d3dDeviceContext;

            D3D11CreateDevice(
                dxgiAdapter3,
                D3D_DRIVER_TYPE_UNKNOWN,     // UNKNOWN if pAdapter is not null
                0,                           // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
                d3dDeviceFlags,
                d3dFeatureLevels,
                ARRAYSIZE(d3dFeatureLevels),
                D3D11_SDK_VERSION,           // Always set this to D3D11_SDK_VERSION for Windows Store apps. Do I need to?
                &d3dDevice,
                &useless_feature_level_variable,
                &d3dDeviceContext
            );
            COP_ASSERT(useless_feature_level_variable == D3D_FEATURE_LEVEL_11_1);

            d3dDevice->QueryInterface(&m_device);
            d3dDeviceContext->QueryInterface(&m_deviceContext);

            d3dDevice->Release();
            d3dDeviceContext->Release();
        }

        dxgiAdapter3->Release();
    }


#if COP_ENABLE_GRAPHICS_API_DEBUG

    void DX11GraphicsDevice::_DebugLayer_Init()
    {
        DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiInfoQueue));
        m_dxgiInfoQueue->ClearRetrievalFilter(k_DxgiDebugGuid);
        m_dxgiInfoQueue->ClearStorageFilter(k_DxgiDebugGuid);

        // Filter create/destroy resource messages, they probably can be useful, but most of the time it's just a spam
        DXGI_INFO_QUEUE_MESSAGE_ID dxgiDenyMessageIDs[] = {
            D3D11_MESSAGE_ID_LIVE_OBJECT_SUMMARY,
            D3D11_MESSAGE_ID_CREATE_BUFFER,
            D3D11_MESSAGE_ID_CREATE_TEXTURE2D,
            D3D11_MESSAGE_ID_CREATE_SHADERRESOURCEVIEW,
            D3D11_MESSAGE_ID_CREATE_RENDERTARGETVIEW,
            D3D11_MESSAGE_ID_CREATE_DEPTHSTENCILVIEW,
            D3D11_MESSAGE_ID_CREATE_VERTEXSHADER,
            D3D11_MESSAGE_ID_CREATE_PIXELSHADER,
            D3D11_MESSAGE_ID_CREATE_INPUTLAYOUT,
            D3D11_MESSAGE_ID_CREATE_FENCE,
            D3D11_MESSAGE_ID_DESTROY_BUFFER,
            D3D11_MESSAGE_ID_DESTROY_TEXTURE2D,
            D3D11_MESSAGE_ID_DESTROY_SHADERRESOURCEVIEW,
            D3D11_MESSAGE_ID_DESTROY_RENDERTARGETVIEW,
            D3D11_MESSAGE_ID_DESTROY_DEPTHSTENCILVIEW,
            D3D11_MESSAGE_ID_DESTROY_VERTEXSHADER,
            D3D11_MESSAGE_ID_DESTROY_PIXELSHADER,
            D3D11_MESSAGE_ID_DESTROY_INPUTLAYOUT,
            D3D11_MESSAGE_ID_DESTROY_FENCE,
        };
        DXGI_INFO_QUEUE_FILTER dxgiInfoQueueFilter = {
            .DenyList = {
                .NumIDs  = ARRAYSIZE(dxgiDenyMessageIDs),
                .pIDList = dxgiDenyMessageIDs,
            },
        };

        m_dxgiInfoQueue->PushRetrievalFilter(k_DxgiDebugGuid, &dxgiInfoQueueFilter);
        m_dxgiInfoQueue->PushStorageFilter(k_DxgiDebugGuid, &dxgiInfoQueueFilter);
    }

    void DX11GraphicsDevice::_DebugLayer_ReportLiveObjects()
    {
        // Need to cast these manually because `DEFINE_ENUM_FLAG_OPERATORS(DXGI_DEBUG_RLO_FLAGS)` not defined for some reason
        int32 dxgiDebugRloFlags = DXGI_DEBUG_RLO_SUMMARY;
        dxgiDebugRloFlags |= DXGI_DEBUG_RLO_IGNORE_INTERNAL;

        IDXGIDebug1* dxgiDevice;
        DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDevice)); // DOCS: Flags are not used(there is none of them)
        dxgiDevice->ReportLiveObjects(k_DxgiDebugGuid, static_cast<DXGI_DEBUG_RLO_FLAGS>(dxgiDebugRloFlags));
        dxgiDevice->Release();

        COP_DX11_LOG_DEBUG_LAYER();
    }

    void DX11GraphicsDevice::_DebugLayer_LogMessages()
    {
        const uint64 messageCount = m_dxgiInfoQueue->GetNumStoredMessages(k_DxgiDebugGuid);

        uint64 allocatedMessageSize = 0;
        uint8* dxgiMessageMemory = nullptr;

        for (uint64 i = 0; i < messageCount; ++i)
        {
            uint64 messageSize = 0;
            m_dxgiInfoQueue->GetMessageW(k_DxgiDebugGuid, i, nullptr, &messageSize);

            if (messageSize > allocatedMessageSize)
            {
                allocatedMessageSize = messageSize;
                delete[] dxgiMessageMemory;
                dxgiMessageMemory = new uint8[messageSize];
            }

            DXGI_INFO_QUEUE_MESSAGE* dxgiMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(dxgiMessageMemory);
            // NOTE(v.matushkin): I do not understand how logging works with char* message when I'm calling GetMessageW
            m_dxgiInfoQueue->GetMessageW(k_DxgiDebugGuid, i, dxgiMessage, &messageSize);

            // TODO(v.matushkin): <Debug/Log/Severity> May be LOG macro should depend on d3dMessage->Severity ?
            //  May be also log category and ID as strings
            COP_LOG_DEBUG("DX11 DEBUG: {:.{}}", dxgiMessage->pDescription, dxgiMessage->DescriptionByteLength);
        }

        delete[] dxgiMessageMemory;

        m_dxgiInfoQueue->ClearStoredMessages(k_DxgiDebugGuid);
    }

#endif // COP_ENABLE_GRAPHICS_API_DEBUG

} // namespace Copium
