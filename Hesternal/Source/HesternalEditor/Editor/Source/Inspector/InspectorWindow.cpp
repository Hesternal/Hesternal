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

            if (Transform* const transform = entityManager.TryGetComponent<Transform>(entity))
            {
                TransformGui(transform).Draw();
            }

            if (LocalToWorld* const localToWorld = entityManager.TryGetComponent<LocalToWorld>(entity))
            {
                LocalToWorldGui(localToWorld).Draw();
            }

            if (Parent* const parent = entityManager.TryGetComponent<Parent>(entity))
            {
                ParentGui(parent).Draw();
            }

            if (Child* const child = entityManager.TryGetComponent<Child>(entity))
            {
                ChildGui(child).Draw();
            }

            if (DirectionalLight* const directionalLight = entityManager.TryGetComponent<DirectionalLight>(entity))
            {
                DirectionalLightGui(directionalLight).Draw();
            }

            if (PointLight* const pointLight = entityManager.TryGetComponent<PointLight>(entity))
            {
                PointLightGui(pointLight).Draw();
            }

            if (SpotLight* const spotLight = entityManager.TryGetComponent<SpotLight>(entity))
            {
                SpotLightGui(spotLight).Draw();
            }

            if (Camera* const camera = entityManager.TryGetComponent<Camera>(entity))
            {
                CameraGui(camera).Draw();
            }

            if (Controller* const controller = entityManager.TryGetComponent<Controller>(entity))
            {
                ControllerGui(controller).Draw();
            }

            if (RenderMesh* const renderMesh = entityManager.TryGetComponent<RenderMesh>(entity))
            {
                RenderMeshGui(renderMesh).Draw();
            }
        }
    }

} // namespace Hesternal
