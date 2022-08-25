export module CopiumEngine.Graphics.ICommandBuffer;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Math;


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

        virtual void BeginRenderPass(RenderPassHandle renderPassHandle) = 0;

        // NOTE(v.matushkin): DX11/DX12/Vulkan actually support setting multiple viewports/scissors,
        //  but I don't see any use cases for it right now.
        //  And seems like only Geometry shader can use multiple vieports/scissors ?
        // NOTE(v.matushkin): Do I need to expose min/max depth?
        virtual void SetViewport(const Rect& viewportRect) = 0;
        virtual void SetScissorRect(const RectInt& scissorRect) = 0;

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

        virtual void DrawIndexed(uint32 indexCount, uint32 firstIndex, uint32 vertexOffset) = 0;
        virtual void DrawMesh(MeshHandle meshHandle) = 0;
        virtual void DrawProcedural(uint32 vertexCount) = 0;

    protected:
        ICommandBuffer() = default;
    };

} // export namespace Copium
