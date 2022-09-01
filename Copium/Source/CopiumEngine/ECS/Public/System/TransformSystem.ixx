export module CopiumEngine.ECS.System.Transform;

import CopiumEngine.ECS.ISystem;
import CopiumEngine.ECS.EntityManager;


export namespace Copium
{

    class TransformSystem final : public ISystem
    {
    public:
        TransformSystem() = default;
        ~TransformSystem() = default;

        TransformSystem(TransformSystem&& other) noexcept = default;
        TransformSystem& operator=(TransformSystem&& other) noexcept = default;

        //< ISystem Interface Public
        void OnCreate(EntityManager& entityManager) override;
        void OnDestroy(EntityManager& entityManager) override;
        void OnUpdate(EntityManager& entityManager) override;
        //> ISystem Interface Public
    };

} // export namespace Copium
