export module CopiumEngine.ECS.Scene;

import CopiumEngine.Assets.Model;
import CopiumEngine.ECS.Entity;

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

        void AddModel(const ModelScene* modelScene);

    private:
        std::string         m_sceneName;
        std::vector<Entity> m_rootEntities;
    };

} // export namespace Copium
