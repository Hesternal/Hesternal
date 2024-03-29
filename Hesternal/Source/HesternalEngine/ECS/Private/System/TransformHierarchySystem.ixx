export module HesternalEngine.ECS.System.TransformHierarchy;

import Hesternal.ECS;


export namespace Hesternal
{

    // NOTE(v.matushkin): This is kinda of a hack I guess, so I can edit rotation in ImGui.
    //  Don't know if there is more elegant solution.
    class TransformHierarchySystem final : public ISystem
    {
    public:
        TransformHierarchySystem() = default;
        ~TransformHierarchySystem() = default;

        TransformHierarchySystem(TransformHierarchySystem&& other) noexcept = default;
        TransformHierarchySystem& operator=(TransformHierarchySystem&& other) noexcept = default;

        //< ISystem Interface Public
        void OnCreate(EntityManager& entityManager) override;
        void OnDestroy(EntityManager& entityManager) override;
        void OnUpdate(EntityManager& entityManager) override;
        //> ISystem Interface Public
    };

} // export namespace Hesternal
