export module HesternalEngine.Assets.AssetManager;

import HesternalEngine.Graphics.GraphicsTypes;


export namespace Hesternal
{

    class AssetManager final
    {
    public:
        [[nodiscard]] static ShaderHandle  CreateGpuResource(const ShaderDesc& shaderDesc);
        [[nodiscard]] static TextureHandle CreateGpuResource(const TextureDesc& textureDesc, const SamplerDesc& samplerDesc);

        static void DestroyGpuResource(ShaderHandle shaderHandle);
        static void DestroyGpuResource(TextureHandle textureHandle);
    };

} // export namespace Hesternal
