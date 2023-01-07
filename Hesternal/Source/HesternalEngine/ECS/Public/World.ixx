export module HesternalEngine.ECS.World;

import Hesternal.ECS;

import HesternalEngine.ECS.Scene;

import <memory>;
import <vector>;


export namespace Hesternal
{

    class World final
    {
    public:
        World(std::vector<std::unique_ptr<ISystem>>&& systems);
        ~World();

        World(const World&) = delete;
        World& operator=(const World&) = delete;
        World(World&& other) noexcept = default;
        World& operator=(World&& other) noexcept = default;

        [[nodiscard]] EntityManager& GetEntityManager() { return m_entityManager; }
        [[nodiscard]] Scene* GetDefaultScene() { return m_defaultScene.get(); }

        void Update();

    private:
        EntityManager                         m_entityManager;
        std::vector<std::unique_ptr<ISystem>> m_systems;

        std::unique_ptr<Scene>                m_defaultScene;
    };

} // export namespace Hesternal
