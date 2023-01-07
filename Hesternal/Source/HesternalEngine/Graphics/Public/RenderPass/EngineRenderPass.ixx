export module HesternalEngine.Graphics.EngineRenderPass;

import HesternalEngine.Graphics.GraphicsTypes;
import HesternalEngine.Graphics.RenderGraph;


export namespace Hesternal
{

    class EngineRenderPass final : public IRenderPass
    {
    public:
        EngineRenderPass();
        ~EngineRenderPass() = default;

        EngineRenderPass(EngineRenderPass&& other) noexcept = default;
        EngineRenderPass& operator=(EngineRenderPass&& other) noexcept = default;

        // TODO(v.matushkin): HACK
        [[nodiscard]] RenderTextureHandle GetEngineOutput() const noexcept { return m_engineColorHandle; }

        void OnSchedule(RenderGraph& renderGraph) override;
        void OnCreate(RenderGraph& renderGraph) override;
        void OnRender(RenderContext& renderContext) override;

    private:
        RenderGraphTextureID m_engineColorId;
        RenderGraphTextureID m_engineDepthId;

        RenderPassHandle    m_renderPassHandle;
        RenderTextureHandle m_engineColorHandle;
        RenderTextureHandle m_engineDepthHandle;
    };

} // export namespace Hesternal
