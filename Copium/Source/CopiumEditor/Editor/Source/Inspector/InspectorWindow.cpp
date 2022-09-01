module;

#include "Engine/Core/Defines.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/entity/registry.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEditor.Editor.InspectorWindow;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.ECS.Components;
import CopiumEngine.ECS.Entity;
import CopiumEngine.ECS.EntityManager;
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

            if (Translation* translation = entityManager.TryGetComponent<Translation>(entity); translation != nullptr)
            {
                TranslationGui(translation).Draw();
            }

            if (Rotation* rotation = entityManager.TryGetComponent<Rotation>(entity); rotation != nullptr)
            {
                RotationGui(rotation).Draw();
            }

            if (Scale* scale = entityManager.TryGetComponent<Scale>(entity); scale != nullptr)
            {
                ScaleGui(scale).Draw();
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
