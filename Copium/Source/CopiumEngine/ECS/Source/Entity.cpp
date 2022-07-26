module CopiumEngine.ECS.Entity;

import <string>;
import <utility>;


namespace Copium
{

    Entity::Entity(std::string&& name)
        : m_name(std::move(name))
    {
    }

    Entity::~Entity()
    {
    }

    Entity::Entity(Entity&& other) noexcept
        : m_name(std::move(other.m_name))
        , m_mesh(std::move(other.m_mesh))
        , m_material(std::move(other.m_material))
        , m_children(std::move(other.m_children))
    {
    }

    Entity& Entity::operator=(Entity&& other) noexcept
    {
        m_name = std::move(other.m_name);
        m_mesh = std::move(other.m_mesh);
        m_material = std::move(other.m_material);
        m_children = std::move(other.m_children);
        return *this;
    }


    void Entity::Convert(IBinaryConverter& bc)
    {
        bc << m_name;
        bc << m_children;
    }

    void Entity::OnAfterDeserealize()
    {
    }

} // namespace Copium
