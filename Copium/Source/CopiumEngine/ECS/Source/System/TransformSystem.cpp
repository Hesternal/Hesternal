module;

#include "Copium/Core/Defines.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/container/dense_map.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEngine.ECS.System.Transform;

import Copium.Core;
import Copium.Math;

import CopiumEngine.ECS.Components;


namespace Copium
{

    void TransformSystem::OnCreate(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);
    }

    void TransformSystem::OnDestroy(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);
    }

    void TransformSystem::OnUpdate(EntityManager& entityManager)
    {
        const auto transformView = entityManager.GetView<const Transform, LocalToWorld>();

        for (const auto&& [entity, transform, localToWorld] : transformView.each())
        {
            localToWorld.Value = Math::TRS(transform.Position, transform.Rotation, transform.Scale);
        }
    }

} // namespace Copium
