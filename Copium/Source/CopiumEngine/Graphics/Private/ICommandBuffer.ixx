module;

#include "Copium/Core/Defines.hpp"

export module CopiumEngine.Graphics.ICommandBuffer;

import Copium.Core;
import Copium.Math;

import CopiumEngine.Graphics.GraphicsTypes;

import <string_view>;


export namespace Copium
{

    struct ICommandBuffer
    {
    public:
        virtual ~ICommandBuffer() = default;

        ICommandBuffer(const ICommandBuffer& other) = delete;
        ICommandBuffer& operator=(const ICommandBuffer& other) = delete;
        ICommandBuffer(ICommandBuffer&& other) = default;
        ICommandBuffer& operator=(ICommandBuffer&& other) = default;

        //- RenderPass
        virtual void BeginRenderPass(RenderPassHandle renderPassHandle) = 0;

        // NOTE(v.matushkin): DX11/DX12/Vulkan actually support setting multiple viewports/scissors,
        //  but I don't see any use cases for it right now.
        //  And seems like only Geometry shader can use multiple vieports/scissors ?
        // NOTE(v.matushkin): Do I need to expose min/max depth?
        virtual void SetViewport(const Rect& viewportRect) = 0;
        virtual void SetScissorRect(const RectInt& scissorRect) = 0;

        //- Bind
        virtual void BindShader(ShaderHandle shaderHandle) = 0;
        virtual void BindVertexBuffer(GraphicsBufferHandle vertexBufferHandle, uint32 stride, uint32 offset) = 0;
        virtual void BindIndexBuffer(GraphicsBufferHandle indexBufferHandle, IndexFormat indexFormat) = 0;
        // TODO(v.matushkin): Shouldn't be exposed, set buffers only through material?
        virtual void BindConstantBuffer(GraphicsBufferHandle constantBufferHandle, uint32 slot) = 0;
        virtual void BindConstantBuffer(GraphicsBufferHandle constantBufferHandle, uint32 slot, uint32 elementIndex, uint32 elementSize) = 0;
        // NOTE(v.matushkin): May be rework Texture/RenderTexture so that they use the same handle type?
        virtual void BindTexture(TextureHandle textureHandle, uint32 slot) = 0;
        virtual void BindTexture(RenderTextureHandle renderTextureHandle, uint32 slot) = 0;
        virtual void BindMaterial(TextureHandle baseColorTextureHandle, TextureHandle normalTextureHandle) = 0;

        //- Draw
        virtual void DrawIndexed(uint32 indexCount, uint32 firstIndex, uint32 vertexOffset) = 0;
        virtual void DrawMesh(MeshHandle meshHandle) = 0;
        virtual void DrawProcedural(uint32 vertexCount) = 0;

        //- GraphicsBuffer
        virtual void CopyBuffer(GraphicsBufferHandle srcGraphicsBufferHandle, GraphicsBufferHandle dstGraphicsBufferHandle) = 0;
        [[nodiscard]] virtual void* MapBuffer(GraphicsBufferHandle graphicsBufferHandle) = 0;
        virtual void UnmapBuffer(GraphicsBufferHandle graphicsBufferHandle) = 0;

#if COP_ENABLE_GRAPHICS_API_DEBUG
        virtual void BeginSample(std::string_view name) = 0;
        virtual void EndSample() = 0;
#endif

    protected:
        ICommandBuffer() = default;
    };

} // export namespace Copium
