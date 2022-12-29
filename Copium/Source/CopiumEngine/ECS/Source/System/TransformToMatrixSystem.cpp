module;

#include "Copium/Core/Defines.hpp"
#include "Copium/ECS/EnttErrorsWorkaround.hpp"

module CopiumEngine.ECS.System.TransformToMatrix;

import Copium.Core;
import Copium.Math;

import CopiumEngine.ECS.Components;


namespace Copium
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

} // namespace Copium
