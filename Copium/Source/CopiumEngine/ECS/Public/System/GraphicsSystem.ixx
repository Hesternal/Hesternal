export module CopiumEngine.ECS.System.Graphics;

import Copium.ECS;


export namespace Copium
{

    class GraphicsSystem final : public ISystem
    {
    public:
        GraphicsSystem() = default;
        ~GraphicsSystem() = default;

        GraphicsSystem(GraphicsSystem&& other) noexcept = default;
        GraphicsSystem& operator=(GraphicsSystem&& other) noexcept = default;

        //< ISystem Interface Public
        void OnCreate(EntityManager& entityManager) override;
        void OnDestroy(EntityManager& entityManager) override;
        void OnUpdate(EntityManager& entityManager) override;
        //> ISystem Interface Public
    };

} // export namespace Copium
