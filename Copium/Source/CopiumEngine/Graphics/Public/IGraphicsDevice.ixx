export module CopiumEngine.Graphics.IGraphicsDevice;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Graphics.ICommandBuffer;
import CopiumEngine.Math;

import <memory>;
import <span>;


export namespace Copium
{

    struct IGraphicsDevice
    {
        virtual ~IGraphicsDevice() = default;

        IGraphicsDevice(const IGraphicsDevice& other) = delete;
        IGraphicsDevice& operator=(const IGraphicsDevice& other) = delete;
        IGraphicsDevice(IGraphicsDevice&& other) = default;
        IGraphicsDevice& operator=(IGraphicsDevice&& other) = default;

        [[nodiscard]] virtual RenderTextureHandle GetSwapchainRenderTexture(SwapchainHandle swapchainHandle) = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        [[nodiscard]] virtual std::unique_ptr<ICommandBuffer> GetCommandBuffer() = 0;

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

    protected:
        // NOTE(v.matushkin): Shoud be in BlendStateDesc
        // NOTE(v.matushkin): Need a lot of checks to support it
        static constexpr bool k_IndependentBlendEnable = false;
    };

} // export namespace Copium
