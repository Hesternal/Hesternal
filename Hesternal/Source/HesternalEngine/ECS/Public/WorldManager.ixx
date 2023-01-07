export module HesternalEngine.ECS.WorldManager;

import HesternalEngine.ECS.World;

import <memory>;


export namespace Hesternal
{

    class WorldManager final
    {
    public:
        static void Init();
        static void Shutdown();

        [[nodiscard]] static World* GetDefaultWorld() { return m_defaulWorld.get(); }

    public:
        static inline std::unique_ptr<World> m_defaulWorld;
    };

} // export namespace Hesternal
