export module CopiumEngine.ECS.Scene;

import CopiumEngine.Assets.Model;
import CopiumEngine.ECS.Entity;

import <vector>;


export namespace Copium
{

    class Scene final
    {
    public:
        Scene() = default;
        ~Scene();

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        Scene(Scene&& other) noexcept = default;
        Scene& operator=(Scene&& other) noexcept = default;

        void AddModel(const ModelScene* modelScene);

    private:
        std::vector<Entity> m_rootEntities;
    };

} // export namespace Copium
