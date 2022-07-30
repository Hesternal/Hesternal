export module CopiumEngine.ECS.WorldManager;

import CopiumEngine.ECS.World;

import <memory>;


export namespace Copium
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

} // export namespace Copium
