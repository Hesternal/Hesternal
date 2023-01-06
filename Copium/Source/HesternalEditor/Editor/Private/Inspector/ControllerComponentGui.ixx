module;

#include <imgui.h>

export module CopiumEditor.Editor.ControllerComponentGui;

import CopiumEngine.ECS.Components;

import CopiumEditor.GUI.ComponentGui;
import CopiumEditor.GUI.EditorGui;


export namespace Copium
{

    class ControllerGui : public ComponentGui
    {
    public:
        ControllerGui(Controller* controller)
            : ComponentGui("Controller")
            , m_controller(controller)
        {
        }

        ~ControllerGui() = default;

        ControllerGui(ControllerGui&& other) noexcept = default;
        ControllerGui& operator=(ControllerGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            EditorGui::FloatField("CameraSpeed", m_controller->CameraSpeed);
            EditorGui::FloatField("CameraBoost", m_controller->CameraBoost);

            ImGui::BeginDisabled(true);
            EditorGui::FloatField("Yaw", m_controller->Yaw);
            EditorGui::FloatField("Pitch", m_controller->Pitch);
            ImGui::EndDisabled();
        }

    private:
        Controller* m_controller;
    };

} // export namespace Copium
