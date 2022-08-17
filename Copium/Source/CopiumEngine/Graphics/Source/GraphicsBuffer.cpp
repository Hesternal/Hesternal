module;

#include "Engine/Core/Debug.hpp"

module CopiumEngine.Graphics.GraphicsBuffer;

import CopiumEngine.Graphics;
import CopiumEngine.Graphics.GraphicsTypes;

import <cstring>;
import <utility>;


namespace Copium
{

    GraphicsBuffer::GraphicsBuffer(const GraphicsBufferDesc& graphicsBufferDesc)
        : GraphicsBuffer(graphicsBufferDesc, std::span<const uint8>())
    {
    }

    GraphicsBuffer::GraphicsBuffer(const GraphicsBufferDesc& graphicsBufferDesc, std::span<const uint8> initialData)
        : m_graphicsBufferDesc(graphicsBufferDesc)
        , m_sizeInBytes(m_graphicsBufferDesc.SizeInBytes())
    {
        COP_ASSERT_MSG(m_graphicsBufferDesc.ElementCount > 0, "Trying to create GraphicsBuffer with ElementCount < 1");
        COP_ASSERT_MSG(m_graphicsBufferDesc.ElementSize > 0, "Trying to create GraphicsBuffer with ElementSize < 1");

        COP_ASSERT_MSG(m_graphicsBufferDesc.Usage != GraphicsBufferUsage::Index
                       || (m_graphicsBufferDesc.ElementSize == 2 || m_graphicsBufferDesc.ElementSize == 4),
                       "For Index GraphicsBuffer ElementSize should be either 2 or 4");

        m_graphicsBufferData = std::make_unique<uint8[]>(m_sizeInBytes);

        std::span<const uint8> graphicsBufferData;

        if (initialData.empty() == false)
        {
            COP_ASSERT_MSG(initialData.size() <= m_sizeInBytes, "data is bigger than GraphicsBuffer can hold");

            graphicsBufferData = std::span<const uint8>(m_graphicsBufferData.get(), m_sizeInBytes);
            std::memcpy(m_graphicsBufferData.get(), initialData.data(), initialData.size());
        }

        m_graphicsBufferHandle = Graphics::GetGraphicsDevice()->CreateGraphicsBuffer(m_graphicsBufferDesc, graphicsBufferData);
    }

    GraphicsBuffer::~GraphicsBuffer()
    {
        if (m_graphicsBufferHandle != GraphicsBufferHandle::Invalid)
        {
            Graphics::GetGraphicsDevice()->DestroyGraphicsBuffer(m_graphicsBufferHandle);
        }
    }

    GraphicsBuffer::GraphicsBuffer(GraphicsBuffer&& other) noexcept
        : m_graphicsBufferDesc(other.m_graphicsBufferDesc)
        , m_graphicsBufferData(std::move(other.m_graphicsBufferData))
        , m_sizeInBytes(std::exchange(other.m_sizeInBytes, 0))
        , m_graphicsBufferHandle(std::exchange(other.m_graphicsBufferHandle, GraphicsBufferHandle::Invalid))
    {
    }

    GraphicsBuffer& GraphicsBuffer::operator=(GraphicsBuffer&& other) noexcept
    {
        m_graphicsBufferDesc = other.m_graphicsBufferDesc;
        m_graphicsBufferData = std::move(other.m_graphicsBufferData);
        m_sizeInBytes = std::exchange(other.m_sizeInBytes, 0);
        m_graphicsBufferHandle = std::exchange(other.m_graphicsBufferHandle, GraphicsBufferHandle::Invalid);
        return *this;
    }


    void GraphicsBuffer::SetData(std::span<const uint8> data)
    {
        COP_ASSERT(data.empty() == false);
        COP_ASSERT_MSG(data.size() <= m_sizeInBytes, "data is bigger than GraphicsBuffer can hold");

        std::memcpy(m_graphicsBufferData.get(), data.data(), data.size());

        _UpdateGpuResource();
    }

    std::span<uint8> GraphicsBuffer::Map()
    {
        return std::span<uint8>(m_graphicsBufferData.get(), m_sizeInBytes);
    }

    void GraphicsBuffer::Unmap()
    {
        _UpdateGpuResource();
    }


    void GraphicsBuffer::_UpdateGpuResource() const
    {
        std::span<const uint8> graphicsBufferData(m_graphicsBufferData.get(), m_sizeInBytes);
        Graphics::GetGraphicsDevice()->UpdateGraphicsBuffer(m_graphicsBufferHandle, graphicsBufferData);
    }

} // namespace Copium
