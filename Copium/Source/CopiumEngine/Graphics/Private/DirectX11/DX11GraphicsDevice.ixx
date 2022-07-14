module;

#include "Engine/Core/Defines.hpp"

#include <d3d11_4.h>
#if COP_ENABLE_GRAPHICS_API_DEBUG
    #include <dxgidebug.h>
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

#include <unordered_map>

// TODO(v.matushkin): Need to make this module private
export module CopiumEngine.Graphics.DX11GraphicsDevice;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Graphics.IGraphicsDevice;


export namespace Copium
{

    class DX11GraphicsDevice final : public IGraphicsDevice
    {
        struct DX11Shader
        {
            ID3D11VertexShader* VertexShader;
            ID3D11PixelShader*  PixelShader;
        };

        struct DX11Swapchain
        {
            IDXGISwapChain4*        Swapchain;
            ID3D11Texture2D1*       Texture;     // NOTE(v.matushkin): No need to save it?
            ID3D11RenderTargetView* Rtv;
            DXGI_FORMAT             Format;
            DXGI_SWAP_CHAIN_FLAG    Flags;
            uint8                   BufferCount;
        };

    public:
        DX11GraphicsDevice();
        ~DX11GraphicsDevice();

        DX11GraphicsDevice(const DX11GraphicsDevice&) = delete;
        DX11GraphicsDevice& operator=(const DX11GraphicsDevice&) = delete;
        DX11GraphicsDevice(DX11GraphicsDevice&& other) noexcept = default;
        DX11GraphicsDevice& operator=(DX11GraphicsDevice&& other) noexcept = default;

        void BeginFrame() override;
        void EndFrame() override;

        void BindShader(ShaderHandle shaderHandle) override;

        void DrawProcedural(uint32 vertexCount) override;

        [[nodiscard]] ShaderHandle    CreateShader(const ShaderDesc& shaderDesc) override;
        [[nodiscard]] SwapchainHandle CreateSwapchain(const SwapchainDesc& swapchainDesc) override;

        void ResizeSwapchain(SwapchainHandle swapchainHandle, uint16 width, uint16 height) override;

        void DestroyShader(ShaderHandle shaderHandle) override;
        void DestroySwapchain(SwapchainHandle swapchainHandle) override;

    private:
        void _CreateDevice();

#if COP_ENABLE_GRAPHICS_API_DEBUG
        void _DebugLayer_Init();
        void _DebugLayer_ReportLiveObjects();
        void _DebugLayer_LogMessages();
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

    private:
        IDXGIFactory5*                                     m_factory;
        ID3D11Device5*                                     m_device;
        ID3D11DeviceContext4*                              m_deviceContext;

#if COP_ENABLE_GRAPHICS_API_DEBUG
        IDXGIInfoQueue*                                    m_dxgiInfoQueue;
#endif // COP_ENABLE_GRAPHICS_API_DEBUG

        std::unordered_map<ShaderHandle,    DX11Shader>    m_shaders;
        std::unordered_map<SwapchainHandle, DX11Swapchain> m_swapchains;
    };

} // export namespace Copium
