module;

#include "Hesternal/Core/Debug.hpp"

#include <d3d11_4.h>
#if HS_ENABLE_GRAPHICS_API_DEBUG
    #include <dxgidebug.h>
#endif // HS_ENABLE_GRAPHICS_API_DEBUG

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get 'import std;'
HS_WARNING_PUSH
HS_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
HS_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
module HesternalEngine.Graphics.DX11GraphicsDevice;
HS_WARNING_POP

import HesternalEngine.Graphics.DX11Conversion;
import HesternalEngine.Graphics.DXCommon;

import <cstring>;
import <memory>;
import <string>;
import <unordered_map>;
import <utility>;


// TODO(v.matushkin):
//  - <Device/Lost>
//    Handle ID3D11Device5::RegisterDeviceRemovedEvent


#define RELEASE_COM_PTR(ptr) if (ptr != nullptr) ptr->Release()

// NOTE(v.matushkin): May be add macro for FAILED(HRESULT) ?
//  Because right now I only log in EndFrame() and ~DX11GraphicsDevice()
//  What if some d3d/dxgi call failed earlier(than EndFrame() call) and the app crashed
#if HS_ENABLE_GRAPHICS_API_DEBUG
    #define HS_DX11_LOG_DEBUG_LAYER() _DebugLayer_LogMessages()
#else
    #define HS_DX11_LOG_DEBUG_LAYER() do { } while((void)0,0)
#endif // HS_ENABLE_GRAPHICS_API_DEBUG


namespace
{
    using namespace Hesternal;


#if HS_ENABLE_GRAPHICS_API_DEBUG
    static const GUID k_DxgiDebugGuid = DXGI_DEBUG_ALL;
#endif // HS_ENABLE_GRAPHICS_API_DEBUG


    static constinit uint32 g_GraphicsBufferHandleWorkaround = 0;
    static constinit uint32 g_MeshHandleWorkaround           = 0;
    static constinit uint32 g_RenderPassHandleWorkaround     = 0;
    static constinit uint32 g_RenderTextureHandleWorkaround  = 0;
    static constinit uint32 g_ShaderHandleWorkaround         = 0;
    static constinit uint32 g_SwapchainHandleWorkaround      = 0;
    static constinit uint32 g_TextureHandleWorkaround        = 0;


#if HS_ENABLE_GRAPHICS_API_DEBUG_NAMES
    static void SetResourceName(ID3D11DeviceChild* d3dResource, const std::string& name) noexcept
    {
        const uint32 nameLength = static_cast<uint32>(name.length());
        d3dResource->SetPrivateData(WKPDID_D3DDebugObjectName, nameLength, name.data());
    }
#else
    static void SetResourceName([[maybe_unused]] ID3D11DeviceChild* d3dResource, [[maybe_unused]] const std::string& name) noexcept {}
#endif // HS_ENABLE_GRAPHICS_API_DEBUG_NAMES

} // namespace


namespace Hesternal
{

    void DX11GraphicsBuffer::Release()
    {
        Buffer->Release();
        RELEASE_COM_PTR(SRV);
    }

    void DX11RenderTexture::Release()
    {
        Texture->Release();
        RELEASE_COM_PTR(RTV);
        RELEASE_COM_PTR(DSV);
        RELEASE_COM_PTR(SRV);
    }

    void DX11Shader::Release()
    {
        InputLayout->Release();
        VertexShader->Release();
        RasterizerState->Release();
        PixelShader->Release();
        DepthStencilState->Release();
        BlendState->Release();
    }

    void DX11Swapchain::Release(DX11GraphicsDevice* dx11GraphicsDevice)
    {
        dx11GraphicsDevice->DestroyRenderTexture(SwapchainRTHandle);

        // NOTE(v.matushkin): IDXGISwapChain->Release() can lead to errors if swapchain will be recreated from the same HWND right after this call,
        //  because swapchain destruction is deffered?
        Swapchain->Release();
    }

    void DX11Texture2D::Release()
    {
        Texture->Release();
        SRV->Release();
        Sampler->Release();
    }


    DX11GraphicsDevice::DX11GraphicsDevice()
        : m_factory(nullptr)
        , m_device(nullptr)
        , m_deviceContext(nullptr)
#if HS_ENABLE_GRAPHICS_API_DEBUG
        , m_dxgiInfoQueue(nullptr)
#endif // HS_ENABLE_GRAPHICS_API_DEBUG
        , m_renderTextureSampler(nullptr)
    {
        _CreateDevice();
#if HS_ENABLE_GRAPHICS_API_DEBUG
        _DebugLayer_Init();
#endif // HS_ENABLE_GRAPHICS_API_DEBUG

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
        HS_LOG_WARN_COND(m_graphicsBuffers.size() != 0, "{:d} GraphicsBuffer(s) were not cleaned up before DX11GraphicsDevice destruction", m_graphicsBuffers.size());
        for (auto& handleAndGraphicsBuffer : m_graphicsBuffers)
        {
            handleAndGraphicsBuffer.second.Release();
        }
        //- RenderPasses
        HS_LOG_WARN_COND(m_renderPasses.size() != 0, "{:d} RenderPass(es) were not cleaned up before DX11GraphicsDevice destruction", m_renderPasses.size());
        //- Swapchains (Should be destroyed before RenderTextures)
        HS_LOG_WARN_COND(m_swapchains.size() != 0, "{:d} Swapchain(s) were not cleaned up before DX11GraphicsDevice destruction", m_swapchains.size());
        for (auto& handleAndSwapchain : m_swapchains)
        {
            handleAndSwapchain.second.Release(this);
        }
        //- RenderTextures
        HS_LOG_WARN_COND(m_renderTextures.size() != 0, "{:d} RenderTexture(s) were not cleaned up before DX11GraphicsDevice destruction", m_renderTextures.size());
        for (auto& handleAndRenderTexture : m_renderTextures)
        {
            handleAndRenderTexture.second.Release();
        }
        //- Shaders
        HS_LOG_WARN_COND(m_shaders.size() != 0, "{:d} Shader(s) were not cleaned up before DX11GraphicsDevice destruction", m_shaders.size());
        for (auto& handleAndShader : m_shaders)
        {
            handleAndShader.second.Release();
        }
        //- Textures
        HS_LOG_WARN_COND(m_textures.size() != 0, "{:d} Texture(s) were not cleaned up before DX11GraphicsDevice destruction", m_textures.size());
        for (auto& handleAndTexture2D : m_textures)
        {
            handleAndTexture2D.second.Release();
        }

        RELEASE_COM_PTR(m_renderTextureSampler);

        // NOTE(v.matushkin): https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-flush
        // NOTE(v.matushkin): Probably this should be called for debug only
        m_deviceContext->ClearState();
        m_deviceContext->Flush();
        RELEASE_COM_PTR(m_deviceContext);

#if HS_ENABLE_GRAPHICS_API_DEBUG
        _DebugLayer_ReportLiveObjects();
        RELEASE_COM_PTR(m_dxgiInfoQueue);
#endif // HS_ENABLE_GRAPHICS_API_DEBUG

        RELEASE_COM_PTR(m_device);
        RELEASE_COM_PTR(m_factory);
    }


    RenderTextureHandle DX11GraphicsDevice::GetSwapchainRenderTexture(SwapchainHandle swapchainHandle)
    {
        return _GetSwapchain(swapchainHandle).SwapchainRTHandle;
    }


    void DX11GraphicsDevice::BeginFrame()
    {
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

        HS_DX11_LOG_DEBUG_LAYER();
    }


    std::unique_ptr<ICommandBuffer> DX11GraphicsDevice::GetCommandBuffer()
    {
        return std::make_unique<DX11CommandBuffer>(m_deviceContext, this);
    }


    GraphicsBufferHandle DX11GraphicsDevice::CreateGraphicsBuffer(const GraphicsBufferDesc& graphicsBufferDesc, std::span<const uint8> initialData)
    {
        DX11GraphicsBuffer dx11GraphicsBuffer;

        //- Create Buffer
        {
            const DX11GraphicsBufferFlags dx11GraphicsBufferFlags = dx11_GraphicsBufferFlags(graphicsBufferDesc.Usage);

            // NOTE(v.matushkin): May be set it in every case?
            const uint32 structureByteStride = graphicsBufferDesc.Usage == GraphicsBufferUsage::Structured
                                             ? graphicsBufferDesc.ElementSize
                                             : 0;

            // NOTE(v.matushkin): Hardcoding D3D11_USAGE_DYNAMIC and D3D11_CPU_ACCESS_WRITE
            //  as there is no way to tell if the buffer will be updated later or it will be IMMUTABLE
            const D3D11_BUFFER_DESC d3dBufferDesc = {
                .ByteWidth           = graphicsBufferDesc.SizeInBytes(),
                .Usage               = D3D11_USAGE_DYNAMIC,
                .BindFlags           = dx11GraphicsBufferFlags.Bind,
                .CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE,
                .MiscFlags           = dx11GraphicsBufferFlags.Misc,
                .StructureByteStride = structureByteStride,
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
        //- Create Buffer SRV
        if (graphicsBufferDesc.Usage == GraphicsBufferUsage::Structured)
        {
            // NOTE(v.matushkin): Seems like you supposed to use always use FirstElement and NumElements
            //   if this is true: https://stackoverflow.com/questions/26544489/d3d11-buffer-srv-how-do-i-use-it
            const D3D11_BUFFER_SRV d3dBufferSrv = {
                .FirstElement = 0,
                .NumElements  = graphicsBufferDesc.ElementCount,
            };
            const D3D11_SHADER_RESOURCE_VIEW_DESC1 d3dSrvDesc = {
                .Format        = DXGI_FORMAT_UNKNOWN,
                .ViewDimension = D3D11_SRV_DIMENSION_BUFFER,
                .Buffer        = d3dBufferSrv,
            };
            m_device->CreateShaderResourceView1(dx11GraphicsBuffer.Buffer, &d3dSrvDesc, &dx11GraphicsBuffer.SRV);
        }
        else
        {
            dx11GraphicsBuffer.SRV = nullptr;
        }

        const auto graphicsBufferHandle = static_cast<GraphicsBufferHandle>(g_GraphicsBufferHandleWorkaround++);
        m_graphicsBuffers.emplace(graphicsBufferHandle, dx11GraphicsBuffer);

        return graphicsBufferHandle;
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
            HS_ASSERT_MSG(depthStencilAttachment != RenderTextureHandle::Invalid, "Trying to use DepthStencilAttachment in Subpass while there is no DepthStencil RenderTexture in RenderPass");

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

            SetResourceName(dx11RenderTexture.RTV, renderTextureDesc.Name);
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

            SetResourceName(dx11RenderTexture.DSV, renderTextureDesc.Name);
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

            SetResourceName(dx11RenderTexture.SRV, renderTextureDesc.Name);
        }
        else
        {
            dx11RenderTexture.SRV = nullptr;
        }

        SetResourceName(dx11RenderTexture.Texture, renderTextureDesc.Name);

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

            HS_LOG_ERROR_COND(dx11Shader.RasterizerState == nullptr, "Failed to create ID3D11RasterizerState2 for '{}' shader", shaderDesc.Name);
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

            HS_LOG_ERROR_COND(dx11Shader.DepthStencilState == nullptr, "Failed to create ID3D11DepthStencilState for '{}' shader", shaderDesc.Name);
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

            HS_LOG_ERROR_COND(dx11Shader.BlendState == nullptr, "Failed to create ID3D11BlendState1 for '{}' shader", shaderDesc.Name);
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

        SetResourceName(dx11Shader.VertexShader, shaderDesc.Name);
        SetResourceName(dx11Shader.PixelShader, shaderDesc.Name);

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

        dx11Swapchain.SwapchainRTHandle = static_cast<RenderTextureHandle>(g_RenderTextureHandleWorkaround++);
        m_renderTextures.emplace(dx11Swapchain.SwapchainRTHandle, dx11SwapchainRenderTexture);

        const auto swapchainHandle = static_cast<SwapchainHandle>(g_SwapchainHandleWorkaround++);
        m_swapchains.emplace(swapchainHandle, dx11Swapchain);

        return swapchainHandle;
    }

    TextureHandle DX11GraphicsDevice::CreateTexture2D(const TextureDesc& textureDesc, const SamplerDesc& samplerDesc)
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
            D3D11_SAMPLER_DESC d3dSamplerDesc = {
                .Filter         = dx11_SamplerFilter(samplerDesc.MinFilter, samplerDesc.MagFilter, samplerDesc.MipmapFilter, samplerDesc.AnisoLevel),
                .AddressU       = dx11_SamplerAddressMode(samplerDesc.AddressModeU),
                .AddressV       = dx11_SamplerAddressMode(samplerDesc.AddressModeV),
                .AddressW       = dx11_SamplerAddressMode(samplerDesc.AddressModeW),
                .MipLODBias     = samplerDesc.MipLodBias,
                .MaxAnisotropy  = samplerDesc.AnisoLevel,
                .ComparisonFunc = D3D11_COMPARISON_NEVER, // NOTE(v.matushkin): Only for Depth textures?
                // .BorderColor      // NOTE(v.matushkin): Only used if D3D11_TEXTURE_ADDRESS_BORDER is specified in Address*
                .MinLOD         = 0,
                .MaxLOD         = D3D11_FLOAT32_MAX,
            };
            m_device->CreateSamplerState(&d3dSamplerDesc, &dx11Texture2D.Sampler);
        }

        SetResourceName(dx11Texture2D.Texture, textureDesc.Name);
        SetResourceName(dx11Texture2D.SRV, textureDesc.Name);

        const auto textureHandle = static_cast<TextureHandle>(g_TextureHandleWorkaround++);
        m_textures.emplace(textureHandle, dx11Texture2D);

        return textureHandle;
    }


    void DX11GraphicsDevice::ResizeSwapchain(SwapchainHandle swapchainHandle, uint16 width, uint16 height)
    {
        DX11Swapchain& dx11Swapchain = _GetSwapchain(swapchainHandle);
        DX11RenderTexture& dx11SwapchainRenderTexture = _GetRenderTexture(dx11Swapchain.SwapchainRTHandle);

        //- Release old Swapchain RenderTexture
        dx11SwapchainRenderTexture.RTV->Release();
        dx11SwapchainRenderTexture.Texture->Release();

        dx11Swapchain.Swapchain->ResizeBuffers(dx11Swapchain.BufferCount, width, height, dx11Swapchain.Format, dx11Swapchain.Flags);

        //- Get new Swapchain RenderTexture
        dx11Swapchain.Swapchain->GetBuffer(0, IID_PPV_ARGS(&dx11SwapchainRenderTexture.Texture));
        m_device->CreateRenderTargetView1(dx11SwapchainRenderTexture.Texture, nullptr, &dx11SwapchainRenderTexture.RTV);
    }


    void DX11GraphicsDevice::DestroyGraphicsBuffer(GraphicsBufferHandle graphicsBufferHandle)
    {
        auto graphicsBufferMapNode = m_graphicsBuffers.extract(graphicsBufferHandle);
        HS_ASSERT(graphicsBufferMapNode.empty() == false);

        graphicsBufferMapNode.mapped().Release();
    }

    void DX11GraphicsDevice::DestroyRenderPass(RenderPassHandle renderPassHandle)
    {
        auto renderPassesMapNode = m_renderPasses.extract(renderPassHandle);
        HS_ASSERT(renderPassesMapNode.empty() == false);
    }

    void DX11GraphicsDevice::DestroyRenderTexture(RenderTextureHandle renderTextureHandle)
    {
        auto renderTexturesMapNode = m_renderTextures.extract(renderTextureHandle);
        HS_ASSERT(renderTexturesMapNode.empty() == false);

        renderTexturesMapNode.mapped().Release();
    }

    void DX11GraphicsDevice::DestroyShader(ShaderHandle shaderHandle)
    {
        auto shadersMapNode = m_shaders.extract(shaderHandle);
        HS_ASSERT(shadersMapNode.empty() == false);

        shadersMapNode.mapped().Release();
    }

    void DX11GraphicsDevice::DestroySwapchain(SwapchainHandle swapchainHandle)
    {
        auto swapchainsMapNode = m_swapchains.extract(swapchainHandle);
        HS_ASSERT(swapchainsMapNode.empty() == false);

        swapchainsMapNode.mapped().Release(this);
    }

    void DX11GraphicsDevice::DestroyTexture2D(TextureHandle textureHandle)
    {
        auto texturesMapNode = m_textures.extract(textureHandle);
        HS_ASSERT(texturesMapNode.empty() == false);

        texturesMapNode.mapped().Release();
    }


    void DX11GraphicsDevice::_CreateDevice()
    {
        //- Create Factory and Adapter
        IDXGIAdapter3* dxgiAdapter3;
        {
            uint32 dxgiFactoryFlags = 0;
#if HS_ENABLE_GRAPHICS_API_DEBUG
            // NOTE(v.matushkin): Looks like I need to set this to use IDXGIInfoQueue?
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif // HS_ENABLE_GRAPHICS_API_DEBUG

            IDXGIAdapter1* dxgiAdapter;

            CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory));
            m_factory->EnumAdapters1(0, &dxgiAdapter);
            dxgiAdapter->QueryInterface(&dxgiAdapter3);
            dxgiAdapter->Release();

            //-- Get GPU description
#if HS_ENABLE_LOGGING
            DXGI_ADAPTER_DESC2 dxgiAdapterDesc;
            dxgiAdapter3->GetDesc2(&dxgiAdapterDesc);
            HS_LOG_INFO(L"DirectX11 Adapter: {}", dxgiAdapterDesc.Description);
#endif // HS_ENABLE_LOGGING
        }

        //- Create Device and DeviceContext
        {
            // NOTE(v.matushkin): Do I even need this?
            //  This flag adds support for surfaces with a color-channel ordering different
            //  from the API default. It is required for compatibility with Direct2D.
            uint32 d3dDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if HS_ENABLE_GRAPHICS_API_DEBUG
            d3dDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // HS_ENABLE_GRAPHICS_API_DEBUG

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
            HS_ASSERT(useless_feature_level_variable == D3D_FEATURE_LEVEL_11_1);

            d3dDevice->QueryInterface(&m_device);
            d3dDeviceContext->QueryInterface(&m_deviceContext);

            d3dDevice->Release();
            d3dDeviceContext->Release();
        }

        dxgiAdapter3->Release();
    }


#if HS_ENABLE_GRAPHICS_API_DEBUG

    void DX11GraphicsDevice::_DebugLayer_Init()
    {
        DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiInfoQueue));
        m_dxgiInfoQueue->ClearRetrievalFilter(k_DxgiDebugGuid);
        m_dxgiInfoQueue->ClearStorageFilter(k_DxgiDebugGuid);

        // Filter create/destroy resource messages, they're probably can be useful, but most of the time it's just a spam
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
            D3D11_MESSAGE_ID_CREATE_SAMPLER,
            D3D11_MESSAGE_ID_CREATE_FENCE,
            D3D11_MESSAGE_ID_DESTROY_BUFFER,
            D3D11_MESSAGE_ID_DESTROY_TEXTURE2D,
            D3D11_MESSAGE_ID_DESTROY_SHADERRESOURCEVIEW,
            D3D11_MESSAGE_ID_DESTROY_RENDERTARGETVIEW,
            D3D11_MESSAGE_ID_DESTROY_DEPTHSTENCILVIEW,
            D3D11_MESSAGE_ID_DESTROY_VERTEXSHADER,
            D3D11_MESSAGE_ID_DESTROY_PIXELSHADER,
            D3D11_MESSAGE_ID_DESTROY_INPUTLAYOUT,
            D3D11_MESSAGE_ID_DESTROY_SAMPLER,
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

        HS_DX11_LOG_DEBUG_LAYER();
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
            HS_LOG_DEBUG("DX11 DEBUG: {:.{}}", dxgiMessage->pDescription, dxgiMessage->DescriptionByteLength);
        }

        delete[] dxgiMessageMemory;

        m_dxgiInfoQueue->ClearStoredMessages(k_DxgiDebugGuid);
    }

#endif // HS_ENABLE_GRAPHICS_API_DEBUG

} // namespace Hesternal
