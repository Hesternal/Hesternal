export module CopiumEngine.Graphics.EngineRenderPass;

import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Graphics.RenderGraph;


export namespace Copium
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

} // export namespace Copium
