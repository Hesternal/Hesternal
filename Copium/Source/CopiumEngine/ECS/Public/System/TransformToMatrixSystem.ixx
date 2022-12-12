export module CopiumEngine.ECS.System.TransformToMatrix;

import CopiumEngine.ECS.ISystem;
import CopiumEngine.ECS.EntityManager;


export namespace Copium
{

    class TransformToMatrixSystem final : public ISystem
    {
    public:
        TransformToMatrixSystem() = default;
        ~TransformToMatrixSystem() = default;

        TransformToMatrixSystem(TransformToMatrixSystem&& other) noexcept = default;
        TransformToMatrixSystem& operator=(TransformToMatrixSystem&& other) noexcept = default;

        //< ISystem Interface Public
        void OnCreate(EntityManager& entityManager) override;
        void OnDestroy(EntityManager& entityManager) override;
        void OnUpdate(EntityManager& entityManager) override;
        //> ISystem Interface Public
    };

} // export namespace Copium
