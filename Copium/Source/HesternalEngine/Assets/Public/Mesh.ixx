export module CopiumEngine.Assets.Mesh;

import CopiumEngine.Graphics.GraphicsBuffer;
import CopiumEngine.Graphics.GraphicsTypes;

import <string>;
import <utility>;


export namespace Copium
{

    class Mesh final
    {
    public:
        Mesh(MeshDesc&& meshDesc);
        ~Mesh() = default;

        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        [[nodiscard]] const MeshDesc&       GetDesc()         const noexcept { return m_meshDesc; }
        [[nodiscard]] const std::string&    GetName()         const noexcept { return m_meshDesc.Name; }
        [[nodiscard]] const GraphicsBuffer& GetIndexBuffer()  const noexcept { return m_indexBuffer; }
        [[nodiscard]] const GraphicsBuffer& GetVertexBuffer() const noexcept { return m_vertexBuffer; }

    private:
        MeshDesc       m_meshDesc;
        GraphicsBuffer m_indexBuffer;
        GraphicsBuffer m_vertexBuffer;
    };


    Mesh::Mesh(Mesh&& other) noexcept
        : m_meshDesc(std::move(other.m_meshDesc))
        , m_indexBuffer(std::move(other.m_indexBuffer))
        , m_vertexBuffer(std::move(other.m_vertexBuffer))
    {}

} // export namespace Copium
