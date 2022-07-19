module;

#include <utility>

module CopiumEngine.Assets.Texture;


namespace Copium
{

    Texture::Texture(TextureDesc&& textureDesc, TextureData&& textureData)
        : m_textureDesc(std::move(textureDesc))
        , m_textureData(std::move(textureData))
    {
    }

    Texture::~Texture()
    {
    }

} // namespace Copium
