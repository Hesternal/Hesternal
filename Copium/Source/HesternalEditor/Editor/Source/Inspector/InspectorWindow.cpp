module;

#include "Hesternal/Core/Defines.hpp"
#include "Hesternal/ECS/EnttErrorsWorkaround.hpp"

module HesternalEditor.Editor.InspectorWindow;

import Hesternal.Core;
import Hesternal.ECS;

import HesternalEngine.ECS.Components;
import HesternalEngine.ECS.WorldManager;

import HesternalEditor.Editor.ControllerComponentGui;
import HesternalEditor.Editor.GraphicsComponentGui;
import HesternalEditor.Editor.TransformComponentGui;
import HesternalEditor.GUI.EditorGui;
import HesternalEditor.GUI.Selection;


namespace Hesternal
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

} // namespace Hesternal
