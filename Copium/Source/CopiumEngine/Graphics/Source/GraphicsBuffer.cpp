module;

#include "Copium/Core/Debug.hpp"

module CopiumEngine.Graphics.GraphicsBuffer;

import CopiumEngine.Engine.SystemInfo;
import CopiumEngine.Graphics;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Memory;

import <utility>;


namespace Copium
{

    GraphicsBuffer::GraphicsBuffer(const GraphicsBufferDesc& graphicsBufferDesc, std::span<const uint8> initialData)
        : m_graphicsBufferDesc(graphicsBufferDesc)
    {
        COP_ASSERT_MSG(m_graphicsBufferDesc.ElementCount > 0, "Trying to create GraphicsBuffer with ElementCount < 1");
        COP_ASSERT_MSG(m_graphicsBufferDesc.ElementSize > 0, "Trying to create GraphicsBuffer with ElementSize < 1");

        // NOTE(v.matushkin): No way to check this anymore?
        //if (m_graphicsBufferDesc.Usage == GraphicsBufferUsage::Index)
        //{
        //    COP_ASSERT_MSG(m_graphicsBufferDesc.ElementSize == 2 || m_graphicsBufferDesc.ElementSize == 4,
        //                   "For Index GraphicsBuffer ElementSize should be either 2 or 4");
        //}
        //else
        if (m_graphicsBufferDesc.Usage == GraphicsBufferUsage::Constant)
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

        m_sizeInBytes = graphicsBufferDesc.SizeInBytes(); // Update here because I the ElementSize might be aligned
        COP_ASSERT_MSG(initialData.size() <= m_sizeInBytes, "initialData is bigger than GraphicsBuffer can hold");

        m_graphicsBufferHandle = Graphics::GetGraphicsDevice()->CreateGraphicsBuffer(m_graphicsBufferDesc, initialData);
    }

    GraphicsBuffer::~GraphicsBuffer()
    {
        if (m_graphicsBufferHandle != GraphicsBufferHandle::Invalid)
        {
            Graphics::GetGraphicsDevice()->DestroyGraphicsBuffer(m_graphicsBufferHandle);
            m_graphicsBufferHandle = GraphicsBufferHandle::Invalid;
        }
    }

    GraphicsBuffer::GraphicsBuffer(GraphicsBuffer&& other) noexcept
        : m_graphicsBufferDesc(std::exchange(other.m_graphicsBufferDesc, GraphicsBufferDesc::Invalid()))
        , m_sizeInBytes(std::exchange(other.m_sizeInBytes, 0))
        , m_graphicsBufferHandle(std::exchange(other.m_graphicsBufferHandle, GraphicsBufferHandle::Invalid))
    {}

    GraphicsBuffer& GraphicsBuffer::operator=(GraphicsBuffer&& other) noexcept
    {
        if (this != &other)
        {
            m_graphicsBufferDesc = std::exchange(other.m_graphicsBufferDesc, GraphicsBufferDesc::Invalid());
            m_sizeInBytes = std::exchange(other.m_sizeInBytes, 0);
            m_graphicsBufferHandle = std::exchange(other.m_graphicsBufferHandle, GraphicsBufferHandle::Invalid);
        }
        else
        {
            COP_ASSERT_MSG(false, "Trying to move assign to self");
        }
        return *this;
    }

} // namespace Copium
