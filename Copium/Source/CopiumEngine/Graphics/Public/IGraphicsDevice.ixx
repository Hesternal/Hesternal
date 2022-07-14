export module CopiumEngine.Graphics.IGraphicsDevice;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.GraphicsTypes;


export namespace Copium
{

    struct IGraphicsDevice
    {
        virtual ~IGraphicsDevice() = default;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void BindShader(ShaderHandle shaderHandle) = 0;

        virtual void DrawProcedural(uint32 vertexCount) = 0;

        [[nodiscard]] virtual ShaderHandle    CreateShader(const ShaderDesc& shaderDesc) = 0;
        [[nodiscard]] virtual SwapchainHandle CreateSwapchain(const SwapchainDesc& swapchainDesc) = 0;

        virtual void ResizeSwapchain(SwapchainHandle swapchainHandle, uint16 width, uint16 height) = 0;

        virtual void DestroyShader(ShaderHandle shaderHandle) = 0;
        virtual void DestroySwapchain(SwapchainHandle swapchainHandle) = 0;
    };

} // export namespace Copium
