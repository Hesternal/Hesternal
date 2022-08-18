export module CopiumEngine.Graphics.CommandBuffer;

import CopiumEngine.Assets.Mesh;
import CopiumEngine.Assets.Shader;
import CopiumEngine.Assets.Texture;
import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.ICommandBuffer;
import CopiumEngine.Graphics.GraphicsBuffer;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Math;

import <memory>;
import <utility>;


export namespace Copium
{

    class CommandBuffer final
    {
    public:
        CommandBuffer(std::unique_ptr<ICommandBuffer>&& internalCommandBuffer);
        ~CommandBuffer() = default;

        CommandBuffer(const CommandBuffer& other) = delete;
        CommandBuffer& operator=(const CommandBuffer& other) = delete;
        CommandBuffer(CommandBuffer&& other) = default;
        CommandBuffer& operator=(CommandBuffer&& other) = default;

        void BeginRenderPass(RenderPassHandle renderPassHandle);

        // NOTE(v.matushkin): DX11/DX12/Vulkan actually support setting multiple viewports/scissors,
        //  but I don't see any use cases for it right now.
        //  And seems like only Geometry shader can use multiple vieports/scissors ?
        // NOTE(v.matushkin): Do I need to expose min/max depth?
        void SetViewport(const Rect& viewportRect);
        void SetScissorRect(const RectInt& scissorRect);

        void BindShader(const Shader* shader);
        void BindVertexBuffer(const GraphicsBuffer* vertexBuffer, uint32 stride, uint32 offset);
        void BindIndexBuffer(const GraphicsBuffer* indexBuffer, IndexFormat indexFormat);
        // TODO(v.matushkin): Shouldn't be exposed, set buffers only through material?
        void BindConstantBuffer(const GraphicsBuffer* constantBuffer, uint32 slot);
        // NOTE(v.matushkin): May be rework Texture/RenderTexture so that they use the same handle type?
        void BindTexture(const Texture* texture, uint32 slot);
        void BindTexture(RenderTextureHandle renderTextureHandle, uint32 slot);
        void BindMaterial(const Texture* baseColorTexture, const Texture* normalTexture);

        void DrawIndexed(uint32 indexCount, uint32 firstIndex, uint32 vertexOffset);
        void DrawMesh(const Mesh* mesh);
        void DrawProcedural(uint32 vertexCount);

    private:
        std::unique_ptr<ICommandBuffer> m_commandBuffer;
        ShaderHandle                    m_currentShader;
    };


    CommandBuffer::CommandBuffer(std::unique_ptr<ICommandBuffer>&& internalCommandBuffer)
        : m_commandBuffer(std::move(internalCommandBuffer))
        , m_currentShader(ShaderHandle::Invalid)
    {
    }


    void CommandBuffer::BeginRenderPass(RenderPassHandle renderPassHandle)
    {
        m_commandBuffer->BeginRenderPass(renderPassHandle);
    }


    void CommandBuffer::SetViewport(const Rect& viewportRect)
    {
        m_commandBuffer->SetViewport(viewportRect);
    }

    void CommandBuffer::SetScissorRect(const RectInt& scissorRect)
    {
        m_commandBuffer->SetScissorRect(scissorRect);
    }


    void CommandBuffer::BindShader(const Shader* shader)
    {
        const ShaderHandle shaderHandle = shader->GetHandle();
        if (m_currentShader != shaderHandle)
        {
            m_currentShader = shaderHandle;
            m_commandBuffer->BindShader(shaderHandle);
        }
    }

    void CommandBuffer::BindVertexBuffer(const GraphicsBuffer* vertexBuffer, uint32 stride, uint32 offset)
    {
        m_commandBuffer->BindVertexBuffer(vertexBuffer->GetHandle(), stride, offset);
    }

    void CommandBuffer::BindIndexBuffer(const GraphicsBuffer* indexBuffer, IndexFormat indexFormat)
    {
        m_commandBuffer->BindIndexBuffer(indexBuffer->GetHandle(), indexFormat);
    }

    void CommandBuffer::BindConstantBuffer(const GraphicsBuffer* constantBuffer, uint32 slot)
    {
        m_commandBuffer->BindConstantBuffer(constantBuffer->GetHandle(), slot);
    }

    void CommandBuffer::BindTexture(const Texture* texture, uint32 slot)
    {
        m_commandBuffer->BindTexture(texture->GetHandle(), slot);
    }

    void CommandBuffer::BindTexture(RenderTextureHandle renderTextureHandle, uint32 slot)
    {
        m_commandBuffer->BindTexture(renderTextureHandle, slot);
    }

    void CommandBuffer::BindMaterial(const Texture* baseColorTexture, const Texture* normalTexture)
    {
        m_commandBuffer->BindMaterial(baseColorTexture->GetHandle(), normalTexture->GetHandle());
    }


    void CommandBuffer::DrawIndexed(uint32 indexCount, uint32 firstIndex, uint32 vertexOffset)
    {
        m_commandBuffer->DrawIndexed(indexCount, firstIndex, vertexOffset);
    }

    void CommandBuffer::DrawMesh(const Mesh* mesh)
    {
        m_commandBuffer->DrawMesh(mesh->GetHandle());
    }

    void CommandBuffer::DrawProcedural(uint32 vertexCount)
    {
        m_commandBuffer->DrawProcedural(vertexCount);
    }

} // export namespace CopiumEngine
