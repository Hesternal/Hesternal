export module CopiumEditor.Editor.ImGuiRenderPass;

import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Graphics.IRenderPass;


export namespace Copium
{

    class ImGuiRenderPass final : public IRenderPass
    {
    public:
        ImGuiRenderPass() = default;
        ~ImGuiRenderPass() = default;

        ImGuiRenderPass(ImGuiRenderPass&& other) noexcept = default;
        ImGuiRenderPass& operator=(ImGuiRenderPass&& other) noexcept = default;

        void OnCreate(IGraphicsDevice* graphicsDevice) override;
        void OnRender(const RenderContext& renderContext) override;
    };

} // export namespace Copium
