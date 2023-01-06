export module CopiumEngine.ECS.Scene;

import Copium.ECS;

import CopiumEngine.Assets.Model;

import <string>;
import <vector>;


export namespace Copium
{

    class Scene final
    {
    public:
        Scene(std::string&& sceneName);
        ~Scene();

        Scene(const Scene& other) = delete;
        Scene& operator=(const Scene& other) = delete;
        Scene(Scene&& other) noexcept = default;
        Scene& operator=(Scene&& other) noexcept = default;

        [[nodiscard]] const std::string& GetName() const noexcept { return m_sceneName; }
        [[nodiscard]] const std::vector<Entity>& GetRootEntities() const noexcept { return m_rootEntities; }

        void AddEntity(Entity entity) { m_rootEntities.push_back(entity); }
        void AddModel(const ModelScene* modelScene);

    private:
        std::string         m_sceneName;
        std::vector<Entity> m_rootEntities;
    };

} // export namespace Copium
