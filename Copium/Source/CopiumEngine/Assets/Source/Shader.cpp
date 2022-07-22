module;

#include <utility>

module CopiumEngine.Assets.Shader;


namespace Copium
{

    Shader::Shader(ShaderDesc&& shaderDesc)
        : m_shaderDesc(std::move(shaderDesc))
    {
    }

    Shader::~Shader()
    {
    }

} // namespace Copium
