module;

#include <imgui.h>

module CopiumEditor.GUI.EditorWindow;

import <utility>;


namespace Copium
{

    EditorWindow::EditorWindow(std::string&& windowTitle)
        : m_windowTitle(std::move(windowTitle))
    {
    }


    void EditorWindow::Draw()
    {
        if (ImGui::Begin(m_windowTitle.c_str()))
        {
            OnGui();
        }
        ImGui::End();
    }

} // namespace Copium
