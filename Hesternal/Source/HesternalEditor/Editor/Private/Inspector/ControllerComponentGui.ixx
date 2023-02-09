module;

#include <imgui.h>

export module HesternalEditor.Editor.ControllerComponentGui;

import HesternalEngine.ECS.Components;

import HesternalEditor.GUI.ComponentGui;
import HesternalEditor.GUI.EditorGui;


export namespace Hesternal
{

    class ControllerGui final : public ComponentGui
    {
    public:
        ControllerGui(Controller* controller)
            : ComponentGui("Controller")
            , m_controller(controller)
        {}

        ~ControllerGui() = default;

        ControllerGui(ControllerGui&& other) noexcept = default;
        ControllerGui& operator=(ControllerGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            EditorGui::FieldScalar("CameraSpeed", m_controller->CameraSpeed, 0.0f);
            EditorGui::FieldScalar("CameraBoost", m_controller->CameraBoost, 0.0f);

            ImGui::BeginDisabled(true);
            EditorGui::FieldScalar("Yaw", m_controller->Yaw);
            EditorGui::FieldScalar("Pitch", m_controller->Pitch);
            ImGui::EndDisabled();
        }

    private:
        Controller* m_controller;
    };

} // export namespace Hesternal
