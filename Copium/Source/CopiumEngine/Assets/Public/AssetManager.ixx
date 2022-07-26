export module CopiumEngine.Assets.AssetManager;

import CopiumEngine.Graphics.GraphicsTypes;


export namespace Copium
{

    class AssetManager final
    {
    public:
        [[nodiscard]] static MeshHandle    CreateGpuResource(const MeshDesc& meshDesc);
        [[nodiscard]] static ShaderHandle  CreateGpuResource(const ShaderDesc& shaderDesc);
        [[nodiscard]] static TextureHandle CreateGpuResource(const TextureDesc& textureDesc);

        static void DestroyGpuResource(MeshHandle meshHandle);
        static void DestroyGpuResource(ShaderHandle shaderHandle);
        static void DestroyGpuResource(TextureHandle textureHandle);
    };

} // export namespace Copium
