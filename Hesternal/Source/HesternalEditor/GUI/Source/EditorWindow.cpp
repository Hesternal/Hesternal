module;

#include <imgui.h>

module HesternalEditor.GUI.EditorWindow;

import <utility>;


namespace Hesternal
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

} // namespace Hesternal
