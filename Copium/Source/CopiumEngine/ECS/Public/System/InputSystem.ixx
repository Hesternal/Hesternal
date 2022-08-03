export module CopiumEngine.ECS.System.Input;

import CopiumEngine.ECS.ISystem;
import CopiumEngine.ECS.EntityManager;


export namespace Copium
{

    class InputSystem final : public ISystem
    {
    public:
        InputSystem() = default;
        ~InputSystem() = default;

        InputSystem(InputSystem&& other) noexcept = default;
        InputSystem& operator=(InputSystem&& other) noexcept = default;

        //< ISystem Interface Public
        void OnCreate(EntityManager& entityManager) override;
        void OnDestroy(EntityManager& entityManager) override;
        void OnUpdate(EntityManager& entityManager) override;
        //> ISystem Interface Public
    };

} // export namespace Copium
