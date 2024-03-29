module HesternalEngine.Assets.Material;

import <string>;


namespace Hesternal
{

    Material::Material(std::string&& materialName, const std::shared_ptr<Shader>& shader)
        : m_materialName(std::move(materialName))
        , m_shader(std::move(shader))
    {
    }

    Material::Material(std::string&& materialName, std::shared_ptr<Shader>&& shader)
        : m_materialName(std::move(materialName))
        , m_shader(std::move(shader))
    {
    }

    Material::~Material()
    {
    }

    Material::Material(Material&& other) noexcept
        : m_materialName(std::move(other.m_materialName))
        , m_shader(std::move(other.m_shader))
        , m_baseColorMap(std::move(other.m_baseColorMap))
        , m_metallicMap(std::move(other.m_metallicMap))
        , m_roughnessMap(std::move(other.m_roughnessMap))
        , m_normalMap(std::move(other.m_normalMap))
    {
    }

    Material& Material::operator=(Material&& other) noexcept
    {
        m_materialName = std::move(other.m_materialName);
        m_shader = std::move(other.m_shader);
        m_baseColorMap = std::move(other.m_baseColorMap);
        m_metallicMap = std::move(other.m_metallicMap);
        m_roughnessMap = std::move(other.m_roughnessMap);
        m_normalMap = std::move(other.m_normalMap);
        return *this;
    }


    void Material::Convert(IBinaryConverter& bc)
    {
        bc << m_materialName;
    }

    void Material::OnAfterDeserealize()
    {
    }

} // namespace Hesternal
