module;

#include <imgui.h>

export module HesternalEditor.Editor.ControllerComponentGui;

import HesternalEngine.ECS.Components;

import HesternalEditor.GUI.ComponentGui;
import HesternalEditor.GUI.EditorGui;


export namespace Hesternal
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

} // export namespace Hesternal
