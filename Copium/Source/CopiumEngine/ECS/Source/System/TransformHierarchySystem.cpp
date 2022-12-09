module;

#include "Copium/Core/Defines.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/container/dense_map.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEngine.ECS.System.Transform;

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
        const auto transformView = entityManager.GetView<const Translation, const Rotation, const Scale, LocalToWorld>();

        for (const auto&& [entity, translation, rotation, scale, localToWorld] : transformView.each())
        {
            localToWorld.Value = Math::TRS(translation.Value, rotation.Value, Float3(scale.Value));
        }
    }

} // namespace Copium
