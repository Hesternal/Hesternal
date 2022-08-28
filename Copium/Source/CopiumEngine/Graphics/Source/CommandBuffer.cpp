module CopiumEngine.Graphics.CommandBuffer;

import CopiumEngine.Graphics;


namespace Copium
{

    CommandBuffer::CommandBuffer()
        : m_commandBuffer(Graphics::GetGraphicsDevice()->GetCommandBuffer())
        , m_currentShader(ShaderHandle::Invalid)
    {
    }

} // namespace Copium
