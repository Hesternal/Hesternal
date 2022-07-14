module;

#include "Engine/Core/Debug.hpp"

#include <d3d11_4.h>
#include <d3dcompiler.h>
#if COP_ENABLE_GRAPHICS_API_DEBUG
    #include <dxgidebug.h>
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

#include <unordered_map>
#include <string>
#include <utility>

module CopiumEngine.Graphics.DX11GraphicsDevice;


// TODO(v.matushkin):
//  - <Viewport>
//    Right now I call ID3D11DeviceContext4::RSSetViewports on swapchain resize/creation
//    Not sure that it should be like this. Probably a Camera should manage its viewport.
//
//  - <Device/Lost>
//    Handle ID3D11Device5::RegisterDeviceRemovedEvent


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


#if COP_ENABLE_GRAPHICS_API_DEBUG
    static const GUID k_DxgiDebugGuid = DXGI_DEBUG_ALL;
#endif // COP_ENABLE_GRAPHICS_API_DEBUG


    static uint32 g_ShaderHandleWorkaround    = 0;
    static uint32 g_SwapchainHandleWorkaround = 0;


    // TODO(v.matushkin): Move this to DXCommon
    static DXGI_FORMAT dx11_RenderTextureFormat(RenderTextureFormat renderTextureFormat)
    {
        static const DXGI_FORMAT dxgiRenderTextureFormat[] = {
            DXGI_FORMAT_B8G8R8A8_UNORM,
            DXGI_FORMAT_D32_FLOAT,
        };

        return dxgiRenderTextureFormat[std::to_underlying(renderTextureFormat)];
    }

} // namespace


namespace Copium
{

    DX11GraphicsDevice::DX11GraphicsDevice()
    {
        _CreateDevice();
#if COP_ENABLE_GRAPHICS_API_DEBUG
        _DebugLayer_Init();
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

        //- Set default state
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_deviceContext->GSSetShader(nullptr, nullptr, 0);
        m_deviceContext->HSSetShader(nullptr, nullptr, 0);
        m_deviceContext->DSSetShader(nullptr, nullptr, 0);
        m_deviceContext->CSSetShader(nullptr, nullptr, 0);
    }

    DX11GraphicsDevice::~DX11GraphicsDevice()
    {
#define RELEASE_COM_PTR(ptr) if (ptr != nullptr) ptr->Release()

        //- Shaders
        COP_LOG_WARN_COND(m_shaders.size() != 0, "{:d} shader(s) were not cleaned up before DX11GraphicsDevice destruction", m_shaders.size());
        for (auto& handleAndShader : m_shaders)
        {
            DX11Shader& dx11Shader = handleAndShader.second;
            dx11Shader.VertexShader->Release();
            dx11Shader.PixelShader->Release();
        }

        //- Swapchains
        COP_LOG_WARN_COND(m_swapchains.size() != 0, "{:d} swapchain(s) were not cleaned up before DX11GraphicsDevice destruction", m_swapchains.size());
        for (auto& handleAndSwapchain : m_swapchains)
        {
            DX11Swapchain& dx11Swapchain = handleAndSwapchain.second;
            dx11Swapchain.Rtv->Release();
            dx11Swapchain.Texture->Release();
            dx11Swapchain.Swapchain->Release();
        }

        // NOTE(v.matushkin): https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-flush
        m_deviceContext->ClearState();
        m_deviceContext->Flush();
        RELEASE_COM_PTR(m_deviceContext);

#if COP_ENABLE_GRAPHICS_API_DEBUG
        _DebugLayer_ReportLiveObjects();
        RELEASE_COM_PTR(m_dxgiInfoQueue);
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

        RELEASE_COM_PTR(m_device);
        RELEASE_COM_PTR(m_factory);

#undef RELEASE_COM_PTR
    }


    void DX11GraphicsDevice::BeginFrame()
    {
        // TODO(v.matushkin): Hack
        DX11Swapchain& dx11Swapchain = m_swapchains.begin()->second;

        const float32 clearColor[] = { 0.1f, 0.1f, 0.1f, 0.f };
        m_deviceContext->OMSetRenderTargets(1, &dx11Swapchain.Rtv, nullptr);
        m_deviceContext->ClearRenderTargetView(dx11Swapchain.Rtv, clearColor);
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


    void DX11GraphicsDevice::BindShader(ShaderHandle shaderHandle)
    {
        const auto dx11ShaderIterator = m_shaders.find(shaderHandle);
        COP_ASSERT(dx11ShaderIterator != m_shaders.end());

        const DX11Shader& dx11Shader = dx11ShaderIterator->second;
        m_deviceContext->VSSetShader(dx11Shader.VertexShader, nullptr, 0);
        m_deviceContext->PSSetShader(dx11Shader.PixelShader, nullptr, 0);
    }


    void DX11GraphicsDevice::DrawProcedural(uint32 vertexCount)
    {
        m_deviceContext->Draw(vertexCount, 0);
    }


    ShaderHandle DX11GraphicsDevice::CreateShader(const ShaderDesc& shaderDesc)
    {
        static const char* k_ShaderEntryPoint   = "main";
        static const char* k_VertexShaderTarget = "vs_5_0";
        static const char* k_PixelShaderTarget  = "ps_5_0";

        ID3DBlob* d3dVertexShaderBlob;
        {
            ID3DBlob* d3dShaderStageErrorBlob;

            // TODO(v.matushkin): D3DCompile2 ?
            const auto d3dCompileResult = D3DCompile(
                shaderDesc.VertexSource.data(),
                shaderDesc.VertexSource.length(),
                "LOL",                // NOTE(v.matushkin): Can be nullptr, don't know where/how it will be used
                nullptr,
                nullptr,              // TODO(v.matushkin): I need to use this ID3DInclude
                k_ShaderEntryPoint,
                k_VertexShaderTarget,
                0,                    // TODO(v.matushkin): Use this, especially D3DCOMPILE_OPTIMIZATION_LEVEL* and may be D3DCOMPILE_PACK_MATRIX_ROW_MAJOR
                0,
                &d3dVertexShaderBlob,
                &d3dShaderStageErrorBlob
            );
            if (FAILED(d3dCompileResult))
            {
                // TODO(v.matushkin): For D3DX11CompileFromFile, vertexErrorBlob == nullptr means shader file wasn't found,
                //  so with D3DCompile this cannot happen?
                COP_ASSERT(d3dShaderStageErrorBlob != nullptr); // NOTE(v.matushkin): Could this ever happen?
                COP_LOG_ERROR("Shader stage compilation error: {}", static_cast<char*>(d3dShaderStageErrorBlob->GetBufferPointer()));
                d3dShaderStageErrorBlob->Release();
            }
        }

        ID3DBlob* d3dPixelShaderBlob;
        {
            ID3DBlob* d3dShaderStageErrorBlob;

            // TODO(v.matushkin): D3DCompile2 ?
            const auto d3dCompileResult = D3DCompile(
                shaderDesc.FragmentSource.data(),
                shaderDesc.FragmentSource.length(),
                "LOL",               // NOTE(v.matushkin): Can be nullptr, don't know where/how it will be used
                nullptr,
                nullptr,             // TODO(v.matushkin): I need to use this ID3DInclude
                k_ShaderEntryPoint,
                k_PixelShaderTarget,
                0,                   // TODO(v.matushkin): Use this, especially D3DCOMPILE_OPTIMIZATION_LEVEL* and may be D3DCOMPILE_PACK_MATRIX_ROW_MAJOR
                0,
                &d3dPixelShaderBlob,
                &d3dShaderStageErrorBlob
            );
            if (FAILED(d3dCompileResult))
            {
                // TODO(v.matushkin): For D3DX11CompileFromFile, vertexErrorBlob == nullptr means shader file wasn't found,
                //  so with D3DCompile this cannot happen?
                COP_ASSERT(d3dShaderStageErrorBlob != nullptr); // NOTE(v.matushkin): Could this ever happen?
                COP_LOG_ERROR("Shader stage compilation error: {}", static_cast<char*>(d3dShaderStageErrorBlob->GetBufferPointer()));
                d3dShaderStageErrorBlob->Release();
            }
        }

        ID3D11VertexShader* d3dVertexShader;
        ID3D11PixelShader*  d3dPixelShader;

        m_device->CreateVertexShader(d3dVertexShaderBlob->GetBufferPointer(), d3dVertexShaderBlob->GetBufferSize(), nullptr, &d3dVertexShader);
        m_device->CreatePixelShader(d3dPixelShaderBlob->GetBufferPointer(), d3dPixelShaderBlob->GetBufferSize(), nullptr, &d3dPixelShader);

        d3dVertexShaderBlob->Release();
        d3dPixelShaderBlob->Release();

        const auto shaderHandle = static_cast<ShaderHandle>(g_ShaderHandleWorkaround++);
        m_shaders.insert(std::pair(
            shaderHandle,
            DX11Shader{
                .VertexShader = d3dVertexShader,
                .PixelShader  = d3dPixelShader,
            }
        ));

        return shaderHandle;
    }

    SwapchainHandle DX11GraphicsDevice::CreateSwapchain(const SwapchainDesc& swapchainDesc)
    {
        IDXGISwapChain4*        dxgiSwapchain4;
        ID3D11Texture2D1*       d3dRenderTexture;
        ID3D11RenderTargetView* d3dRtv;

        const DXGI_FORMAT dxgiSwapchainFormat = dx11_RenderTextureFormat(swapchainDesc.Format);
        // TODO(v.matushkin): I guess at least I should use DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH flag?
        const DXGI_SWAP_CHAIN_FLAG dxgiSwapchainFlags = static_cast<DXGI_SWAP_CHAIN_FLAG>(0);

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
                .Format      = dxgiSwapchainFormat,
                .Stereo      = false,
                .SampleDesc  = dxgiSwapchainSampleDesc,
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = swapchainDesc.BufferCount,
                .Scaling     = DXGI_SCALING_STRETCH,            // TODO(v.matushkin): Play with this
                .SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD,
                .AlphaMode   = DXGI_ALPHA_MODE_IGNORE,          // NOTE(v.matushkin): Don't know
                .Flags       = dxgiSwapchainFlags,
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
            dxgiSwapchain1->QueryInterface(&dxgiSwapchain4);
            dxgiSwapchain1->Release();
        }

        //- Get Swapchain RenderTexture
        {
            dxgiSwapchain4->GetBuffer(0, IID_PPV_ARGS(&d3dRenderTexture));
            // NOTE(v.matushkin): ViewDesc?
            m_device->CreateRenderTargetView(d3dRenderTexture, nullptr, &d3dRtv);
        }

        //- Setup Viewport
        {
            // TODO(v.matushkin): <Viewport>
            D3D11_VIEWPORT d3dViewport = {
                .Width    = static_cast<float32>(swapchainDesc.Width),
                .Height   = static_cast<float32>(swapchainDesc.Height),
                .MinDepth = 0,
                .MaxDepth = 1,
            };
            m_deviceContext->RSSetViewports(1, &d3dViewport);
        }

        const auto swapchainHandle = static_cast<SwapchainHandle>(g_SwapchainHandleWorkaround++);
        m_swapchains.insert(std::pair(
            swapchainHandle,
            DX11Swapchain{
                .Swapchain   = dxgiSwapchain4,
                .Texture     = d3dRenderTexture,
                .Rtv         = d3dRtv,
                .Format      = dxgiSwapchainFormat,
                .Flags       = dxgiSwapchainFlags,
                .BufferCount = swapchainDesc.BufferCount,
            }
        ));

        return swapchainHandle;
    }


    void DX11GraphicsDevice::ResizeSwapchain(SwapchainHandle swapchainHandle, uint16 width, uint16 height)
    {
        const auto dx11SwapchainIterator = m_swapchains.find(swapchainHandle);
        COP_ASSERT(dx11SwapchainIterator != m_swapchains.end());

        DX11Swapchain& dx11Swapchain = dx11SwapchainIterator->second;
        dx11Swapchain.Rtv->Release();
        dx11Swapchain.Texture->Release();
        dx11Swapchain.Swapchain->ResizeBuffers(dx11Swapchain.BufferCount, width, height, dx11Swapchain.Format, dx11Swapchain.Flags);

        //- Get Swapchain RenderTexture
        {
            dx11Swapchain.Swapchain->GetBuffer(0, IID_PPV_ARGS(&dx11Swapchain.Texture));
            m_device->CreateRenderTargetView(dx11Swapchain.Texture, nullptr, &dx11Swapchain.Rtv);
        }

        //- Setup Viewport
        {
            // TODO(v.matushkin): <Viewport>
            D3D11_VIEWPORT d3dViewport = {
                .Width    = static_cast<float32>(width),
                .Height   = static_cast<float32>(height),
                .MinDepth = 0,
                .MaxDepth = 1,
            };
            m_deviceContext->RSSetViewports(1, &d3dViewport);
        }
    }


    void DX11GraphicsDevice::DestroyShader(ShaderHandle shaderHandle)
    {
        auto shaderNode = m_shaders.extract(shaderHandle);
        COP_ASSERT(shaderNode.empty() == false);

        DX11Shader& dx11Shader = shaderNode.mapped();
        dx11Shader.VertexShader->Release();
        dx11Shader.PixelShader->Release();
    }

    void DX11GraphicsDevice::DestroySwapchain(SwapchainHandle swapchainHandle)
    {
        auto swapchainNode = m_swapchains.extract(swapchainHandle);
        COP_ASSERT(swapchainNode.empty() == false);

        DX11Swapchain& dx11Swapchain = swapchainNode.mapped();
        dx11Swapchain.Rtv->Release();
        dx11Swapchain.Texture->Release();
        // NOTE(v.matushkin): This can lead to errors if swapchain will be recreated from the same HWND right after this call,
        //  because swapchain destruction is deffered?
        dx11Swapchain.Swapchain->Release();
    }


    void DX11GraphicsDevice::_CreateDevice()
    {
        //- Create Factory and Adapter
        IDXGIAdapter3* dxgiAdapter3;
        {
            uint32 dxgiFactoryFlags = 0;
#if COP_ENABLE_GRAPHICS_API_DEBUG
            // NOTE(v.matushkin): Look like I need to set this to use IDXGIInfoQueue?
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

        DXGI_INFO_QUEUE_MESSAGE_ID dxgiDenyMessageIDs[] = {
            D3D11_MESSAGE_ID_LIVE_OBJECT_SUMMARY,
            D3D11_MESSAGE_ID_CREATE_TEXTURE2D,
            D3D11_MESSAGE_ID_CREATE_RENDERTARGETVIEW,
            D3D11_MESSAGE_ID_CREATE_VERTEXSHADER,
            D3D11_MESSAGE_ID_CREATE_PIXELSHADER,
            D3D11_MESSAGE_ID_CREATE_FENCE,
            D3D11_MESSAGE_ID_DESTROY_TEXTURE2D,
            D3D11_MESSAGE_ID_DESTROY_RENDERTARGETVIEW,
            D3D11_MESSAGE_ID_DESTROY_VERTEXSHADER,
            D3D11_MESSAGE_ID_DESTROY_PIXELSHADER,
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
