module CopiumEngine.Assets.AssetManager;

import CopiumEngine.Graphics;


namespace Copium
{

    MeshHandle AssetManager::CreateGpuResource(const MeshDesc& meshDesc)
    {
        return Graphics::GetGraphicsDevice()->CreateMesh(meshDesc);
    }

    ShaderHandle AssetManager::CreateGpuResource(const ShaderDesc& shaderDesc)
    {
        return Graphics::GetGraphicsDevice()->CreateShader(shaderDesc);
    }

    TextureHandle AssetManager::CreateGpuResource(const TextureDesc& textureDesc)
    {
        return Graphics::GetGraphicsDevice()->CreateTexture2D(textureDesc);
    }


    void AssetManager::DestroyGpuResource(MeshHandle meshHandle)
    {
        Graphics::GetGraphicsDevice()->DestroyMesh(meshHandle);
    }

    void AssetManager::DestroyGpuResource(ShaderHandle shaderHandle)
    {
        Graphics::GetGraphicsDevice()->DestroyShader(shaderHandle);
    }

    void AssetManager::DestroyGpuResource(TextureHandle textureHandle)
    {
        Graphics::GetGraphicsDevice()->DestroyTexture2D(textureHandle);
    }

} // namespace Copium
