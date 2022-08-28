module;

#include "Engine/Core/Debug.hpp"

module CopiumEngine.Graphics.GraphicsBuffer;

import CopiumEngine.Engine.SystemInfo;
import CopiumEngine.Graphics;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Memory;

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
    {
        COP_ASSERT_MSG(m_graphicsBufferDesc.ElementCount > 0, "Trying to create GraphicsBuffer with ElementCount < 1");
        COP_ASSERT_MSG(m_graphicsBufferDesc.ElementSize > 0, "Trying to create GraphicsBuffer with ElementSize < 1");

        if (m_graphicsBufferDesc.Usage == GraphicsBufferUsage::Index)
        {
            COP_ASSERT_MSG(m_graphicsBufferDesc.ElementSize == 2 || m_graphicsBufferDesc.ElementSize == 4,
                           "For Index GraphicsBuffer ElementSize should be either 2 or 4");
        }
        else if (m_graphicsBufferDesc.Usage == GraphicsBufferUsage::Constant)
        {
            COP_ASSERT_MSG(m_graphicsBufferDesc.ElementSize % 4 == 0,
                           "For Constant GraphicsBuffer ElementSize should be a multiply of 4");

            // NOTE(v.matushkin): Make element size to fit alignment requirements, don't know if I actually should do this,
            //  may be there are cases when it is not needed.
            if (m_graphicsBufferDesc.ElementCount > 1)
            {
                m_graphicsBufferDesc.ElementSize = Memory::GetAlignedSize(m_graphicsBufferDesc.ElementSize, SystemInfo::ConstantBufferAlignment);
            }
        }

        m_sizeInBytes = m_graphicsBufferDesc.SizeInBytes();
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
