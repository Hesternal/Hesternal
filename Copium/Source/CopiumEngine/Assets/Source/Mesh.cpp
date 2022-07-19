module;

#include <utility>

module CopiumEngine.Assets.Mesh;


namespace Copium
{

    Mesh::Mesh(MeshDesc&& meshDesc)
        : m_meshDesc(std::move(meshDesc))
    {
    }

    Mesh::~Mesh()
    {
    }

} // namespace Copium
