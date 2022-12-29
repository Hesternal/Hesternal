export module CopiumEngine.ECS.World;

import Copium.ECS;

import CopiumEngine.ECS.Scene;

import <memory>;
import <vector>;


export namespace Copium
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

} // export namespace Copium
