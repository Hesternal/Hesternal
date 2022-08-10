module;

#include "Engine/Core/Defines.hpp"

#include <imgui.h>

module CopiumEditor.Editor.ImGuiRenderPass;

import CopiumEngine.ImGui.ImGuiContext;


namespace Copium
{

    void ImGuiRenderPass::OnCreate(IGraphicsDevice* graphicsDevice)
    {
        COP_UNUSED(graphicsDevice);
    }

    void ImGuiRenderPass::OnRender(const RenderContext& renderContext)
    {
        COP_UNUSED(renderContext);

        ImGuiContext::BeginFrame();

        ImGui::ShowDemoWindow();

        ImGuiContext::EndFrame();
    }

} // namespace Copium
