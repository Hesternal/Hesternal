module;

#include "Copium/Core/Defines.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/container/dense_map.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEngine.ECS.System.Camera;

import Copium.Core;

import CopiumEngine.ECS.Components;
import CopiumEngine.Math;


namespace Copium
{

    void CameraSystem::OnCreate(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);
    }

    void CameraSystem::OnDestroy(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);
    }

    void CameraSystem::OnUpdate(EntityManager& entityManager)
    {
        const auto cameraQuery = entityManager.GetView<const LocalToWorld, Camera>();

        for (const auto&& [cameraEntity, localToWorld, camera] : cameraQuery.each())
        {
            // NOTE(v.matushkin): Assuming there will be no Scale component
            camera.View = Math::InverseRigidTransform(localToWorld.Value);
            camera.ViewProjection = Math::Mul(camera.Projection, camera.View);
        }
    }

} // namespace Copium
