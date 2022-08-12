export module CopiumEngine.Graphics.IGraphicsDevice;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Math;


export namespace Copium
{

    struct IGraphicsDevice
    {
        IGraphicsDevice() = default;
        virtual ~IGraphicsDevice() = default;

        IGraphicsDevice(const IGraphicsDevice&) = delete;
        IGraphicsDevice& operator=(const IGraphicsDevice&) = delete;

        //< For ImGui
        // DX11 ID3D11ShaderResourceView*
        [[nodiscard]] virtual void* GetNativeRenderTexture(RenderTextureHandle renderTextureHandle) = 0;
        //> For ImGui
        [[nodiscard]] virtual RenderTextureHandle GetSwapchainRenderTexture(SwapchainHandle swapchainHandle) = 0;

        virtual void BeginFrame(const Float4x4& objectToWorld, const Float4x4& cameraView, const Float4x4& cameraProjection) = 0;
        virtual void BeginRenderPass(RenderPassHandle renderPassHandle) = 0;
        virtual void EndFrame() = 0;

        virtual void BindShader(ShaderHandle shaderHandle) = 0;
        virtual void BindMaterial(TextureHandle baseColorTextureHandle, TextureHandle normalTextureHandle) = 0;

        virtual void DrawMesh(MeshHandle meshHandle) = 0;
        virtual void DrawProcedural(uint32 vertexCount) = 0;

        [[nodiscard]] virtual MeshHandle CreateMesh(const MeshDesc& meshDesc) = 0;
        [[nodiscard]] virtual RenderPassHandle CreateRenderPass(const RenderPassDesc& renderPassDesc) = 0;
        [[nodiscard]] virtual RenderTextureHandle CreateRenderTexture(const RenderTextureDesc& renderTextureDesc) = 0;
        [[nodiscard]] virtual ShaderHandle CreateShader(const ShaderDesc& shaderDesc) = 0;
        [[nodiscard]] virtual SwapchainHandle CreateSwapchain(const SwapchainDesc& swapchainDesc) = 0;
        [[nodiscard]] virtual TextureHandle CreateTexture2D(const TextureDesc& textureDesc) = 0;

        virtual void ResizeSwapchain(SwapchainHandle swapchainHandle, uint16 width, uint16 height) = 0;

        virtual void DestroyMesh(MeshHandle meshHandle) = 0;
        virtual void DestroyRenderPass(RenderPassHandle renderPassHandle) = 0;
        virtual void DestroyRenderTexture(RenderTextureHandle renderTextureHandle) = 0;
        virtual void DestroyShader(ShaderHandle shaderHandle) = 0;
        virtual void DestroySwapchain(SwapchainHandle swapchainHandle) = 0;
        virtual void DestroyTexture2D(TextureHandle textureHandle) = 0;

    protected:
        // NOTE(v.matushkin): Shoud be in BlendStateDesc
        // NOTE(v.matushkin): Need a lot of checks to support it
        static constexpr bool k_IndependentBlendEnable = false;
    };

} // export namespace Copium
