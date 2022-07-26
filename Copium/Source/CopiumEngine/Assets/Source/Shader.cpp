module;

#include <utility>

module CopiumEngine.Assets.Shader;

import CopiumEngine.Assets.AssetManager;


namespace Copium
{

    Shader::Shader(ShaderDesc&& shaderDesc)
        : m_shaderDesc(std::move(shaderDesc))
    {
        _InitGpuResource();
    }

    Shader::~Shader()
    {
        if (m_shaderHandle != ShaderHandle::Invalid)
        {
            AssetManager::DestroyGpuResource(m_shaderHandle);
        }
    }

    Shader::Shader(Shader&& other) noexcept
        : m_shaderDesc(std::move(other.m_shaderDesc))
        , m_shaderHandle(std::exchange(other.m_shaderHandle, ShaderHandle::Invalid))
    {
    }

    Shader& Shader::operator=(Shader&& other) noexcept
    {
        m_shaderDesc = std::move(other.m_shaderDesc);
        m_shaderHandle = std::exchange(other.m_shaderHandle, ShaderHandle::Invalid);
        return *this;
    }


    void Shader::Convert(IBinaryConverter& bc)
    {
        bc << m_shaderDesc;
    }

    void Shader::OnAfterDeserealize()
    {
        _InitGpuResource();
    }


    void Shader::_InitGpuResource()
    {
        m_shaderHandle = AssetManager::CreateGpuResource(m_shaderDesc);
    }

} // namespace Copium
