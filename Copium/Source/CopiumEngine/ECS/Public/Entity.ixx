export module CopiumEngine.ECS.Entity;

import CopiumEngine.Assets.Material;
import CopiumEngine.Assets.Mesh;
import CopiumEngine.Core.Object;

import <memory>;
import <string>;
import <vector>;
import <utility>;


export namespace Copium
{

    class Entity final : public Object
    {
    public:
        Entity(std::string&& name);
        ~Entity();

        // Default constructor for serialization
        Entity() = default;

        Entity(Entity&& other) noexcept;
        Entity& operator=(Entity&& other) noexcept;

        //< Object Interface Public
        [[nodiscard]] const std::string& GetName() const override { return m_name; }
        //> Object Interface Public

        [[nodiscard]] Mesh* GetMesh() const { return m_mesh.get(); }
        [[nodiscard]] Material* GetMaterial() const { return m_material.get(); }
        [[nodiscard]] const std::vector<Entity>& GetChildren() const { return m_children; }

        void SetMesh(const std::shared_ptr<Mesh>& mesh) { m_mesh = mesh; }
        void SetMesh(std::shared_ptr<Mesh>&& mesh) { m_mesh = std::move(mesh); }
        void SetMaterial(const std::shared_ptr<Material>& material) { m_material = material; }
        void SetMaterial(std::shared_ptr<Material>&& material) { m_material = std::move(material); }

        void AddChild(Entity&& child) { m_children.push_back(std::move(child)); }

    private:
        //< Object Interface Protected
        [[nodiscard]] ClassID GetClassID() const override { return ClassID::Entity; }
        void Convert(IBinaryConverter& bc) override;
        void OnAfterDeserealize() override;
        //> Object Interface Protected

    private:
        std::string               m_name;

        std::shared_ptr<Mesh>     m_mesh;
        std::shared_ptr<Material> m_material;

        std::vector<Entity>       m_children;
    };

} // export namespace Copium
