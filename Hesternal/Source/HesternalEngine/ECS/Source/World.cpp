module HesternalEngine.ECS.World;

import <utility>;


namespace Hesternal
{

    World::World(std::vector<std::unique_ptr<ISystem>>&& systems)
        : m_systems(std::move(systems))
    {
        m_defaultScene = std::make_unique<Scene>("DefaultScene");

        for (const std::unique_ptr<ISystem>& system : m_systems)
        {
            system->OnCreate(m_entityManager);
        }
    }

    World::~World()
    {
        for (const std::unique_ptr<ISystem>& system : m_systems)
        {
            system->OnDestroy(m_entityManager);
        }
    }


    void World::Update()
    {
        for (const std::unique_ptr<ISystem>& system : m_systems)
        {
            system->OnUpdate(m_entityManager);
        }
    }

} // namespace Hesternal
