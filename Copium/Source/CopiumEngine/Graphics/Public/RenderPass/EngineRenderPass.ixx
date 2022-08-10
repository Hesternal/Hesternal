export module CopiumEngine.Graphics.EngineRenderPass;

import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Graphics.IRenderPass;


export namespace Copium
{

    class EngineRenderPass final : public IRenderPass
    {
    public:
        EngineRenderPass(SwapchainHandle swapchainHandle);
        ~EngineRenderPass() = default;

        EngineRenderPass(EngineRenderPass&& other) noexcept = default;
        EngineRenderPass& operator=(EngineRenderPass&& other) noexcept = default;

        void OnCreate(IGraphicsDevice* graphicsDevice) override;
        void OnRender(const RenderContext& renderContext) override;

    private:
        SwapchainHandle     m_swapchainHandle;
        RenderPassHandle    m_renderPassHandle;
        RenderTextureHandle m_depthRTHandle;
    };

} // export namespace Copium
