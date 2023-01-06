module;

#include <imgui.h>

module HesternalEditor.Editor.RenderStatsWindow;


namespace Hesternal
{

    RenderStatsWindow::RenderStatsWindow()
        : EditorWindow("RenderStats")
    {
    }


    void RenderStatsWindow::OnGui()
    {
        const auto framerate = ImGui::GetIO().Framerate;
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
    }

} // namespace Hesternal
