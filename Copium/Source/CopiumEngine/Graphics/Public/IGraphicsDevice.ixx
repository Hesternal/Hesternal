export module CopiumEngine.Graphics.IGraphicsDevice;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Math;

import <span>;


export namespace Copium
{

    struct IGraphicsDevice
    {
        virtual ~IGraphicsDevice() = default;

        IGraphicsDevice(const IGraphicsDevice& other) = delete;
        IGraphicsDevice& operator=(const IGraphicsDevice& other) = delete;

        //< For ImGui
        // DX11 ID3D11ShaderResourceView*
        [[nodiscard]] virtual void* GetNativeRenderTexture(RenderTextureHandle renderTextureHandle) = 0;
        //> For ImGui
        [[nodiscard]] virtual RenderTextureHandle GetSwapchainRenderTexture(SwapchainHandle swapchainHandle) = 0;

        virtual void BeginFrame(const Float4x4& objectToWorld, const Float4x4& cameraView, const Float4x4& cameraProjection) = 0;
        virtual void BeginRenderPass(RenderPassHandle renderPassHandle) = 0;
        virtual void EndFrame() = 0;

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
        // NOTE(v.matushkin): May be rework Texture/RenderTexture so that they use the same handle type?
        virtual void BindTexture(TextureHandle textureHandle, uint32 slot) = 0;
        virtual void BindTexture(RenderTextureHandle renderTextureHandle, uint32 slot) = 0;
        virtual void BindMaterial(TextureHandle baseColorTextureHandle, TextureHandle normalTextureHandle) = 0;

        virtual void DrawIndexed(uint32 indexCount, uint32 firstIndex, uint32 vertexOffset) = 0;
        virtual void DrawMesh(MeshHandle meshHandle) = 0;
        virtual void DrawProcedural(uint32 vertexCount) = 0;

        [[nodiscard]] virtual GraphicsBufferHandle CreateGraphicsBuffer(const GraphicsBufferDesc& graphicsBufferDesc, std::span<const uint8> initialData) = 0;
        [[nodiscard]] virtual MeshHandle CreateMesh(const MeshDesc& meshDesc) = 0;
        [[nodiscard]] virtual RenderPassHandle CreateRenderPass(const RenderPassDesc& renderPassDesc) = 0;
        [[nodiscard]] virtual RenderTextureHandle CreateRenderTexture(const RenderTextureDesc& renderTextureDesc) = 0;
        [[nodiscard]] virtual ShaderHandle CreateShader(const ShaderDesc& shaderDesc) = 0;
        [[nodiscard]] virtual SwapchainHandle CreateSwapchain(const SwapchainDesc& swapchainDesc) = 0;
        [[nodiscard]] virtual TextureHandle CreateTexture2D(const TextureDesc& textureDesc) = 0;

        virtual void UpdateGraphicsBuffer(GraphicsBufferHandle graphicsBufferHandle, std::span<const uint8> data) = 0;
        virtual void ResizeSwapchain(SwapchainHandle swapchainHandle, uint16 width, uint16 height) = 0;

        virtual void DestroyGraphicsBuffer(GraphicsBufferHandle graphicsBufferHandle) = 0;
        virtual void DestroyMesh(MeshHandle meshHandle) = 0;
        virtual void DestroyRenderPass(RenderPassHandle renderPassHandle) = 0;
        virtual void DestroyRenderTexture(RenderTextureHandle renderTextureHandle) = 0;
        virtual void DestroyShader(ShaderHandle shaderHandle) = 0;
        virtual void DestroySwapchain(SwapchainHandle swapchainHandle) = 0;
        virtual void DestroyTexture2D(TextureHandle textureHandle) = 0;

    protected:
        IGraphicsDevice() = default;

        IGraphicsDevice(IGraphicsDevice&& other) = default;
        IGraphicsDevice& operator=(IGraphicsDevice&& other) = default;

    protected:
        // NOTE(v.matushkin): Shoud be in BlendStateDesc
        // NOTE(v.matushkin): Need a lot of checks to support it
        static constexpr bool k_IndependentBlendEnable = false;
    };

} // export namespace Copium
