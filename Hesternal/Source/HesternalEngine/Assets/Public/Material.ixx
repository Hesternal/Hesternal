export module HesternalEngine.Assets.Material;

import HesternalEngine.Assets.Shader;
import HesternalEngine.Assets.Texture;
import HesternalEngine.Core.Object;
import HesternalEngine.Graphics.GraphicsTypes;

import <memory>;
import <string>;
import <utility>;


export namespace Hesternal
{

    class Material final : public Object
    {
    public:
        Material(std::string&& materialName, const std::shared_ptr<Shader>& shader);
        Material(std::string&& materialName, std::shared_ptr<Shader>&& shader);
        ~Material();

        Material(Material&& other) noexcept;
        Material& operator=(Material&& other) noexcept;

        //< Object Interface Public
        [[nodiscard]] const std::string& GetName() const override { return m_materialName; }
        //> Object Interface Public

        [[nodiscard]] const std::shared_ptr<Shader>& GetShader() const { return m_shader; }

        [[nodiscard]] const std::shared_ptr<Texture>& GetBaseColorMap() const { return m_baseColorMap; }
        [[nodiscard]] const std::shared_ptr<Texture>& GetMetallicMap() const { return m_metallicMap; }
        [[nodiscard]] const std::shared_ptr<Texture>& GetRoughnessMap() const { return m_roughnessMap; }
        [[nodiscard]] const std::shared_ptr<Texture>& GetNormalMap() const { return m_normalMap; }

        void SetBaseColorMap(const std::shared_ptr<Texture>& baseColorMap) { m_baseColorMap = baseColorMap; }
        void SetBaseColorMap(std::shared_ptr<Texture>&& baseColorMap) { m_baseColorMap = std::move(baseColorMap); }
        void SetMetallicMap(const std::shared_ptr<Texture>& metallicMap) { m_metallicMap = metallicMap; }
        void SetMetallicMap(std::shared_ptr<Texture>&& metallicMap) { m_metallicMap = std::move(metallicMap); }
        void SetRoughnessMap(const std::shared_ptr<Texture>& roughnessMap) { m_roughnessMap = roughnessMap; }
        void SetRoughnessMap(std::shared_ptr<Texture>&& roughnessMap) { m_roughnessMap = std::move(roughnessMap); }
        void SetNormalMap(const std::shared_ptr<Texture>& normalMap) { m_normalMap = normalMap; }
        void SetNormalMap(std::shared_ptr<Texture>&& normalMap) { m_normalMap = std::move(normalMap); }

    private:
        //< Object Interface Protected
        [[nodiscard]] ClassID GetClassID() const override { return ClassID::Material; }
        void Convert(IBinaryConverter& bc) override;
        void OnAfterDeserealize() override;
        //> Object Interface Protected

    private:
        std::string              m_materialName;
        std::shared_ptr<Shader>  m_shader;

        std::shared_ptr<Texture> m_baseColorMap;
        std::shared_ptr<Texture> m_metallicMap;
        std::shared_ptr<Texture> m_roughnessMap;
        std::shared_ptr<Texture> m_normalMap;
    };

} // export namespace Hesternal
