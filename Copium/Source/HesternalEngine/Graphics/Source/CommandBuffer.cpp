module;

#include "Copium/Core/Debug.hpp"

module HesternalEngine.Graphics.CommandBuffer;

import HesternalEngine.Graphics;


namespace Hesternal
{

    CommandBuffer::CommandBuffer()
        : m_commandBuffer(Graphics::GetGraphicsDevice()->GetCommandBuffer())
        , m_currentShader(ShaderHandle::Invalid)
    {}


    void CommandBuffer::CopyBuffer(const GraphicsBuffer& srcGraphicsBuffer, const GraphicsBuffer& dstGraphicsBuffer)
    {
        const GraphicsBufferHandle srcGraphicsBufferHandle = srcGraphicsBuffer.GetHandle();
        const GraphicsBufferHandle dstGraphicsBufferHandle = dstGraphicsBuffer.GetHandle();

        // TODO(v.matushkin): There is more conditions that I should check
        COP_ASSERT(srcGraphicsBufferHandle != dstGraphicsBufferHandle);
        COP_ASSERT(srcGraphicsBuffer.GetBufferSize() == dstGraphicsBuffer.GetBufferSize());

        m_commandBuffer->CopyBuffer(srcGraphicsBufferHandle, dstGraphicsBufferHandle);
    }

} // namespace Hesternal
