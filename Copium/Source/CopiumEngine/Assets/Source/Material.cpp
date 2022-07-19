module;

#include <string>

module CopiumEngine.Assets.Material;


namespace Copium
{

    Material::Material(std::string&& materialName, std::shared_ptr<Shader> shader)
        : m_materialName(std::move(materialName))
        , m_shader(std::move(shader))
    {
    }

    Material::~Material()
    {
    }

} // namespace Copium
