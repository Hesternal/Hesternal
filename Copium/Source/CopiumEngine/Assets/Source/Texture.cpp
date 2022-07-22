module;

#include <utility>

module CopiumEngine.Assets.Texture;


namespace Copium
{

    Texture::Texture(TextureDesc&& textureDesc)
        : m_textureDesc(std::move(textureDesc))
    {
    }

    Texture::~Texture()
    {
    }

} // namespace Copium
