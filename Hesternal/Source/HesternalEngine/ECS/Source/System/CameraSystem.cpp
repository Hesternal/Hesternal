module;

#include "Hesternal/Core/Defines.hpp"
#include "Hesternal/ECS/EnttErrorsWorkaround.hpp"

module HesternalEngine.ECS.System.Camera;

import Hesternal.Core;
import Hesternal.Math;

import HesternalEngine.ECS.Components;


namespace Hesternal
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

} // namespace Hesternal
