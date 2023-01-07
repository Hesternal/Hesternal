module HesternalEngine.Assets.AssetManager;

import HesternalEngine.Graphics;


namespace Hesternal
{

    ShaderHandle AssetManager::CreateGpuResource(const ShaderDesc& shaderDesc)
    {
        return Graphics::GetGraphicsDevice()->CreateShader(shaderDesc);
    }

    TextureHandle AssetManager::CreateGpuResource(const TextureDesc& textureDesc, const SamplerDesc& samplerDesc)
    {
        return Graphics::GetGraphicsDevice()->CreateTexture2D(textureDesc, samplerDesc);
    }


    void AssetManager::DestroyGpuResource(ShaderHandle shaderHandle)
    {
        Graphics::GetGraphicsDevice()->DestroyShader(shaderHandle);
    }

    void AssetManager::DestroyGpuResource(TextureHandle textureHandle)
    {
        Graphics::GetGraphicsDevice()->DestroyTexture2D(textureHandle);
    }

} // namespace Hesternal
