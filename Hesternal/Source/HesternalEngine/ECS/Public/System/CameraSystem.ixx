export module HesternalEngine.ECS.System.Camera;

import Hesternal.ECS;


export namespace Hesternal
{

    class CameraSystem final : public ISystem
    {
    public:
        CameraSystem() = default;
        ~CameraSystem() = default;

        CameraSystem(CameraSystem&& other) noexcept = default;
        CameraSystem& operator=(CameraSystem&& other) noexcept = default;

        //< ISystem Interface Public
        void OnCreate(EntityManager& entityManager) override;
        void OnDestroy(EntityManager& entityManager) override;
        void OnUpdate(EntityManager& entityManager) override;
        //> ISystem Interface Public
    };

} // export namespace Hesternal
