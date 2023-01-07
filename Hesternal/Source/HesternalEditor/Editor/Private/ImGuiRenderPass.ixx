export module HesternalEditor.Editor.ImGuiRenderPass;

import HesternalEngine.Graphics.GraphicsTypes;
import HesternalEngine.Graphics.RenderGraph;


export namespace Hesternal
{

    class ImGuiRenderPass final : public IRenderPass
    {
    public:
        ImGuiRenderPass();
        ~ImGuiRenderPass() = default;

        ImGuiRenderPass(ImGuiRenderPass&& other) noexcept = default;
        ImGuiRenderPass& operator=(ImGuiRenderPass&& other) noexcept = default;

        void OnSchedule(RenderGraph& renderGraph) override;
        void OnCreate(RenderGraph& renderGraph) override;
        void OnRender(RenderContext& renderContext) override;

    private:
        RenderGraphTextureID m_engineColorId;
        RenderGraphTextureID m_editorColorId;

        SwapchainHandle      m_swapchainHandle;
        RenderPassHandle     m_renderPassHandle;
        RenderTextureHandle  m_engineColorHandle;
        RenderTextureHandle  m_editorColorHandle;
    };

} // export namespace Hesternal
