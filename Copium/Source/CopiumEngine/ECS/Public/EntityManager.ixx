module;

#include "Engine/Core/Defines.hpp"

// warning C4996: 'std::aligned_storage_t': warning STL4034: std::aligned_storage and std::aligned_storage_t are deprecated in C++23.
//  Prefer alignas(T) std::byte t_buff[sizeof(T)].
//  You can define _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING or _SILENCE_ALL_CXX23_DEPRECATION_WARNINGS to acknowledge that you have received this warning.
COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/entity/registry.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

export module CopiumEngine.ECS.EntityManager;

import CopiumEngine.ECS.Components;
import CopiumEngine.ECS.Entity;

import <utility>;


export namespace Copium
{

    class EntityManager final
    {
    public:
        // NOTE(v.matushkin): For some fucking reason =default constructor/destructor doesn't work
        EntityManager();
        ~EntityManager();

        EntityManager(const EntityManager&) = delete;
        EntityManager& operator=(const EntityManager&) = delete;

        EntityManager(EntityManager&& other) noexcept = default;
        EntityManager& operator=(EntityManager&& other) noexcept = default;

        [[nodiscard]] Entity CreateEntity();
        void DestroyEntity(Entity entity);

        template<CComponent TComponent>
        void AddComponent(Entity entity, TComponent&& component)
        {
            m_registry.emplace<TComponent>(entity, std::forward<TComponent>(component));
        }

        template<CComponent... TComponent>
        void RemoveComponent(Entity entity)
        {
            m_registry.erase<TComponent...>(entity);
        }

        template<CComponent... TComponent, CComponent... TExclude>
        [[nodiscard]] entt::basic_view<Entity, entt::get_t<TComponent...>, entt::exclude_t<TExclude...>>
        GetView(entt::exclude_t<TExclude...> exclude = {})
        {
            return m_registry.view<TComponent..., TExclude...>(exclude);
        }

    private:
        entt::basic_registry<Entity> m_registry;
    };

} // export namespace Copium
