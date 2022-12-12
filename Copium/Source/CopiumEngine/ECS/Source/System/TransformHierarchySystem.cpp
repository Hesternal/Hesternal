module;

#include "Copium/Core/Defines.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/container/dense_map.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEngine.ECS.System.TransformHierarchy;

import Copium.Math;

import CopiumEngine.ECS.Entity;
import CopiumEngine.ECS.Components;


namespace Copium
{

    void TransformHierarchySystem::OnCreate(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);
    }

    void TransformHierarchySystem::OnDestroy(EntityManager& entityManager)
    {
        COP_UNUSED(entityManager);
    }

    void TransformHierarchySystem::OnUpdate(EntityManager& entityManager)
    {
        // Recursive lambda, kind of
        class UpdateChildLocalToWorld final
        {
        public:
            UpdateChildLocalToWorld(EntityManager& entityManager)
                : entityManager(entityManager)
                , localToWorldView(entityManager.GetView<LocalToWorld>())
            {}

            void operator()(const Float4x4& parentLocalToWorld, const Entity childEntity)
            {
                Float4x4& localToWorld = localToWorldView.get<LocalToWorld>(childEntity).Value;
                localToWorld = Math::Mul(parentLocalToWorld, localToWorld);

                if (Child* const child = entityManager.TryGetComponent<Child>(childEntity); child != nullptr)
                {
                    for (const Entity entity : child->Children)
                    {
                        operator()(localToWorld, entity);
                    }
                }
            }

        private:
            EntityManager&                                  entityManager;
            decltype(entityManager.GetView<LocalToWorld>()) localToWorldView;
        };

        UpdateChildLocalToWorld updateChildLocalToWorld(entityManager);
        const auto rootsView = entityManager.GetView<const LocalToWorld, const Child>(entt::exclude_t<Parent>());

        for (const auto&& [entity, localToWorld, child] : rootsView.each())
        {
            for (const Entity childEntity : child.Children)
            {
                updateChildLocalToWorld(localToWorld.Value, childEntity);
            }
        }
    }

} // namespace Copium
