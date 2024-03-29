module HesternalEngine.Assets.Texture;

import HesternalEngine.Assets.AssetManager;

import <utility>;


namespace Hesternal
{

    Texture::Texture(TextureDesc&& textureDesc, const SamplerDesc& samplerDesc)
        : m_textureDesc(std::move(textureDesc))
        , m_samplerDesc(samplerDesc)
    {
        _InitGpuResource();
    }

    Texture::~Texture()
    {
        if (m_textureHandle != TextureHandle::Invalid)
        {
            AssetManager::DestroyGpuResource(m_textureHandle);
        }
    }

    Texture::Texture()
        : m_textureHandle(TextureHandle::Invalid)
    {
    }

    Texture::Texture(DoNotInitialize, TextureDesc&& textureDesc)
        : m_textureDesc(std::move(textureDesc))
        , m_textureHandle(TextureHandle::Invalid)
    {
    }

    Texture::Texture(Texture&& other) noexcept
        : m_textureDesc(std::move(other.m_textureDesc))
        , m_textureHandle(std::exchange(other.m_textureHandle, TextureHandle::Invalid))
    {
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        m_textureDesc = std::move(other.m_textureDesc);
        m_textureHandle = std::exchange(other.m_textureHandle, TextureHandle::Invalid);
        return *this;
    }


    void Texture::Convert(IBinaryConverter& bc)
    {
        bc << m_textureDesc;
    }

    void Texture::OnAfterDeserealize()
    {
        _InitGpuResource();
    }


    void Texture::_InitGpuResource()
    {
        m_textureHandle = AssetManager::CreateGpuResource(m_textureDesc, m_samplerDesc);
    }

} // namespace Hesternal
