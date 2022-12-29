module;

#include "Copium/Core/Defines.hpp"
#include "Copium/ECS/EnttErrorsWorkaround.hpp"

module CopiumEngine.ECS.System.Camera;

import Copium.Core;
import Copium.Math;

import CopiumEngine.ECS.Components;


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
