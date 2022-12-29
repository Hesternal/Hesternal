export module Copium.ECS:ISystem;

import :EntityManager;


export namespace Copium
{

    struct ISystem
    {
    public:
        virtual ~ISystem() = default;

        ISystem(const ISystem&) = delete;
        ISystem& operator=(const ISystem&) = delete;
        ISystem(ISystem&& other) noexcept = default;
        ISystem& operator=(ISystem&& other) noexcept = default;

        virtual void OnCreate(EntityManager& entityManager) = 0;
        virtual void OnDestroy(EntityManager& entityManager) = 0;
        virtual void OnUpdate(EntityManager& entityManager) = 0;

    protected:
        ISystem() = default;
    };

} // export namespace Copium
