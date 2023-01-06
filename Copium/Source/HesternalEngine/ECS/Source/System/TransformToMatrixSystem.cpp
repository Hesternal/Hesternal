module;

#include "Hesternal/Core/Defines.hpp"
#include "Hesternal/ECS/EnttErrorsWorkaround.hpp"

module HesternalEngine.ECS.System.TransformToMatrix;

import Hesternal.Core;
import Hesternal.Math;

import HesternalEngine.ECS.Components;


namespace Hesternal
{

    void TransformToMatrixSystem::OnCreate(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);
    }

    void TransformToMatrixSystem::OnDestroy(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);
    }

    void TransformToMatrixSystem::OnUpdate(EntityManager& entityManager)
    {
        const auto transformView = entityManager.GetView<const Transform, LocalToWorld>();

        for (const auto&& [entity, transform, localToWorld] : transformView.each())
        {
            localToWorld.Value = Math::TRS(transform.Position, transform.Rotation, transform.Scale);
        }
    }

} // namespace Hesternal
