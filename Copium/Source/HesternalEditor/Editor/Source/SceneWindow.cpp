module;

#include <imgui.h>

module CopiumEditor.Editor.SceneWindow;

import Copium.Core;

import CopiumEngine.Engine.EngineSettings;
import CopiumEngine.ImGui.ImGuiContext; // For ImGuiTexture


namespace Copium
{

    SceneWindow::SceneWindow(RenderTextureHandle engineOutputHandle)
        : EditorWindow("Scene")
        , m_engineOutputHandle(engineOutputHandle)
    {
    }


    void SceneWindow::OnGui()
    {
        const ImVec2 windowSize = ImGui::GetWindowSize();
        if (ImGui::BeginChild("SceneChild", windowSize))
        {
            ImGuiTexture imguiTexture;
            imguiTexture.Parts.Handle = static_cast<uint32>(m_engineOutputHandle);
            imguiTexture.Parts.IsRenderTexture = true;

            EngineSettings& engineSettings = EngineSettings::Get();
            ImVec2 engineOutputDimensions(engineSettings.RenderWidth, engineSettings.RenderHeight);

            ImGui::Image(reinterpret_cast<ImTextureID>(imguiTexture.Whole), engineOutputDimensions);
        }
        ImGui::EndChild();
    }

} // namespace Copium
