module;

#include "Copium/Core/Defines.hpp"
#include "Copium/ECS/EnttErrorsWorkaround.hpp"

module CopiumEditor.Editor.InspectorWindow;

import Copium.Core;
import Copium.ECS;

import CopiumEngine.ECS.Components;
import CopiumEngine.ECS.WorldManager;

import CopiumEditor.Editor.ControllerComponentGui;
import CopiumEditor.Editor.GraphicsComponentGui;
import CopiumEditor.Editor.TransformComponentGui;
import CopiumEditor.GUI.EditorGui;
import CopiumEditor.GUI.Selection;


namespace Copium
{

    InspectorWindow::InspectorWindow()
        : EditorWindow("Inspector")
    {
    }


    void InspectorWindow::OnGui()
    {
        Entity entity = Selection::GetSelectedEntity();

        if (entity != Entity::Invalid)
        {
            EntityManager& entityManager = WorldManager::GetDefaultWorld()->GetEntityManager();

            if (Transform* transform = entityManager.TryGetComponent<Transform>(entity); transform != nullptr)
            {
                TransformGui(transform ).Draw();
            }

            if (LocalToWorld* localToWorld = entityManager.TryGetComponent<LocalToWorld>(entity); localToWorld != nullptr)
            {
                LocalToWorldGui(localToWorld).Draw();
            }

            if (Parent* parent = entityManager.TryGetComponent<Parent>(entity); parent != nullptr)
            {
                ParentGui(parent).Draw();
            }

            if (Child* child = entityManager.TryGetComponent<Child>(entity); child != nullptr)
            {
                ChildGui(child).Draw();
            }

            if (Camera* camera = entityManager.TryGetComponent<Camera>(entity); camera != nullptr)
            {
                CameraGui(camera).Draw();
            }

            if (Controller* controller = entityManager.TryGetComponent<Controller>(entity); controller != nullptr)
            {
                ControllerGui(controller).Draw();
            }

            if (RenderMesh* renderMesh = entityManager.TryGetComponent<RenderMesh>(entity); renderMesh != nullptr)
            {
                RenderMeshGui(renderMesh).Draw();
            }
        }
    }

} // namespace Copium
