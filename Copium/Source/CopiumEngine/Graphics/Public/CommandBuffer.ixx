module;

#include "Copium/Core/Defines.hpp"

export module CopiumEngine.Graphics.CommandBuffer;

import Copium.Core;
import Copium.Math;

import CopiumEngine.Assets.Mesh;
import CopiumEngine.Assets.Shader;
import CopiumEngine.Assets.Texture;
import CopiumEngine.Graphics.ICommandBuffer;
import CopiumEngine.Graphics.GraphicsBuffer;
import CopiumEngine.Graphics.GraphicsTypes;

import <memory>;
import <span>;
import <utility>;


// https://developercommunity.visualstudio.com/t/VS-2022-1750-Preview-2-C20-modules-b/10230159
#define _COP_NODISCARD_BUG_FIXED false

#if _COP_NODISCARD_BUG_FIXED
    #define _COP_NODISCARD_BUG_WORKAROUND(unused_variable) do { } while ((void)0,0)
#else
    #define _COP_NODISCARD_BUG_WORKAROUND(unused_variable) COP_UNUSED(unused_variable)
#endif


export namespace Copium
{

    class CommandBuffer final
    {
    public:
        CommandBuffer();
        ~CommandBuffer() = default;

        CommandBuffer(const CommandBuffer& other) = delete;
        CommandBuffer& operator=(const CommandBuffer& other) = delete;
        CommandBuffer(CommandBuffer&& other) = default;
        CommandBuffer& operator=(CommandBuffer&& other) = default;

        //- RenderPass
        void BeginRenderPass(RenderPassHandle renderPassHandle);

        void SetViewport(const Rect& viewportRect);
        void SetScissorRect(const RectInt& scissorRect);

        //- Bind
        void BindShader(const Shader* shader);
        void BindVertexBuffer(const GraphicsBuffer* vertexBuffer, uint32 stride, uint32 offset);
        void BindIndexBuffer(const GraphicsBuffer* indexBuffer, IndexFormat indexFormat);
        void BindConstantBuffer(const GraphicsBuffer* constantBuffer, uint32 slot);
        // NOTE(v.matushkin): Not sure about elementIndex param, I've hid the actual parameters cause I don't know
        //  if there are cases when you can bind like half of the buffer or break the 256 byte alignment.
        void BindConstantBuffer(const GraphicsBuffer* constantBuffer, uint32 slot, uint32 elementIndex);
        void BindTexture(TextureHandle textureHandle, uint32 slot);
        void BindTexture(const Texture* texture, uint32 slot);
        void BindTexture(RenderTextureHandle renderTextureHandle, uint32 slot);
        void BindMaterial(const Texture* baseColorTexture, const Texture* normalTexture);

        //- Draw
        void DrawIndexed(uint32 indexCount, uint32 firstIndex, uint32 vertexOffset);
        void DrawMesh(const Mesh* mesh);
        void DrawProcedural(uint32 vertexCount);

        //- GraphicsBuffer
        void CopyBuffer(const GraphicsBuffer& srcGraphicsBuffer, const GraphicsBuffer& dstGraphicsBuffer);
        // NOTE(v.matushkin): May be it's redundant to return span and just a pointer will do?
        [[nodiscard]] std::span<uint8> MapBuffer(const GraphicsBuffer& graphicsBuffer);
        void UnmapBuffer(const GraphicsBuffer& graphicsBuffer);

#if COP_ENABLE_GRAPHICS_API_DEBUG
        void BeginSample(std::string_view name);
        void EndSample();
#else
        void BeginSample([[maybe_unused]] std::string_view name) { _COP_NODISCARD_BUG_WORKAROUND(name); }
        void EndSample() {}
#endif

    private:
        std::unique_ptr<ICommandBuffer> m_commandBuffer;
        ShaderHandle                    m_currentShader;
    };


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

    void CommandBuffer::BindConstantBuffer(const GraphicsBuffer* constantBuffer, uint32 slot, uint32 elementIndex)
    {
        m_commandBuffer->BindConstantBuffer(constantBuffer->GetHandle(), slot, elementIndex, constantBuffer->GetElementSize());
    }

    void CommandBuffer::BindTexture(TextureHandle textureHandle, uint32 slot)
    {
        m_commandBuffer->BindTexture(textureHandle, slot);
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


    std::span<uint8> CommandBuffer::MapBuffer(const GraphicsBuffer& graphicsBuffer)
    {
        void* const mappedData = m_commandBuffer->MapBuffer(graphicsBuffer.GetHandle());
        // NOTE(v.matushkin): GetBufferSize can be different from the actual buffer size
        return std::span<uint8>(static_cast<uint8*>(mappedData), graphicsBuffer.GetBufferSize());
    }

    void CommandBuffer::UnmapBuffer(const GraphicsBuffer& graphicsBuffer)
    {
        m_commandBuffer->UnmapBuffer(graphicsBuffer.GetHandle());
    }


#if COP_ENABLE_GRAPHICS_API_DEBUG
    void CommandBuffer::BeginSample(std::string_view name)
    {
        m_commandBuffer->BeginSample(name);
    }

    void CommandBuffer::EndSample()
    {
        m_commandBuffer->EndSample();
    }
#endif

} // export namespace CopiumEngine
