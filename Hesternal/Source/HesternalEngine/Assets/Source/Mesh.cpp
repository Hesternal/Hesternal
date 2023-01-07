module;

#include "Hesternal/Core/Debug.hpp"

module HesternalEngine.Assets.Mesh;

import Hesternal.Core;

import <span>;
import <utility>;


// NOTE(v.matushkin): I need a way to create IMMUTABLE GraphicsBuffer (altough there is no such thing in DX11/VK ?)


namespace Hesternal
{

    Mesh::Mesh(MeshDesc&& meshDesc)
        : m_meshDesc(std::move(meshDesc))
        , m_indexBuffer(GraphicsBufferDesc{
                .ElementCount = static_cast<uint32>(m_meshDesc.IndexData.size()),
                .ElementSize  = sizeof(uint8),
                .Usage        = GraphicsBufferUsage::Index},
            std::span<const uint8>(m_meshDesc.IndexData.data(), static_cast<uint32>(m_meshDesc.IndexData.size())))
        , m_vertexBuffer(GraphicsBufferDesc::Vertex(static_cast<uint32>(m_meshDesc.VertexData.size()), sizeof(uint8)),
            std::span<const uint8>(m_meshDesc.VertexData.data(), static_cast<uint32>(m_meshDesc.VertexData.size())))
    {}

    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
        if (this != &other)
        {
            m_meshDesc = std::move(other.m_meshDesc);
            m_indexBuffer = std::move(other.m_indexBuffer);
            m_vertexBuffer = std::move(other.m_vertexBuffer);
        }
        else
        {
            COP_ASSERT_MSG(false, "Trying to move assign to self");
        }
        return *this;
    }

} // namespace Hesternal
