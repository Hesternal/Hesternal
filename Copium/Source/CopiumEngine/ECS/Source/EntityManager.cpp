module;

#include "Engine/Core/Defines.hpp"

COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4996)
#include <entt/entity/registry.hpp>
#include <entt/entity/view.hpp>
COP_WARNING_POP

module CopiumEngine.ECS.EntityManager;


namespace Copium
{

    EntityManager::EntityManager()
    {
    }

    EntityManager::~EntityManager()
    {
    }

    // NOTE(v.matushkin): Keep this here in case default move constructor/assignment will just randomly brake

    // EntityManager::EntityManager(EntityManager&& other) noexcept
    //     : m_registry(std::move(other.m_registry))
    // {
    // }
    // 
    // EntityManager& EntityManager::operator=(EntityManager&& other) noexcept
    // {
    //     m_registry = std::move(other.m_registry);
    //     return *this;
    // }


    [[nodiscard]] Entity EntityManager::CreateEntity()
    {
        return m_registry.create();
    }

    void EntityManager::DestroyEntity(Entity entity)
    {
        m_registry.destroy(entity);
    }


} // namespace Copium
