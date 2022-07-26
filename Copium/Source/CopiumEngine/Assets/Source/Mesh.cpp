module;

#include <utility>

module CopiumEngine.Assets.Mesh;

import CopiumEngine.Assets.AssetManager;


namespace Copium
{

    Mesh::Mesh(MeshDesc&& meshDesc)
        : m_meshDesc(std::move(meshDesc))
    {
        _InitGpuResource();
    }

    Mesh::~Mesh()
    {
        if (m_meshHandle != MeshHandle::Invalid)
        {
            AssetManager::DestroyGpuResource(m_meshHandle);
        }
    }

    Mesh::Mesh()
        : m_meshHandle(MeshHandle::Invalid)
    {
    }

    Mesh::Mesh(DoNotInitialize, MeshDesc&& meshDesc)
        : m_meshDesc(std::move(meshDesc))
        , m_meshHandle(MeshHandle::Invalid)
    {
    }

    Mesh::Mesh(Mesh&& other) noexcept
        : m_meshDesc(std::move(other.m_meshDesc))
        , m_meshHandle(std::exchange(other.m_meshHandle, MeshHandle::Invalid))
    {
    }

    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
        m_meshDesc = std::move(other.m_meshDesc);
        m_meshHandle = std::exchange(other.m_meshHandle, MeshHandle::Invalid);
        return *this;
    }


    void Mesh::Convert(IBinaryConverter& bc)
    {
        bc << m_meshDesc;
    }

    void Mesh::OnAfterDeserealize()
    {
        _InitGpuResource();
    }


    void Mesh::_InitGpuResource()
    {
        m_meshHandle = AssetManager::CreateGpuResource(m_meshDesc);
    }

} // namespace Copium
