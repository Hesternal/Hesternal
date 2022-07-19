export module CopiumEngine.Assets.Texture;

import CopiumEngine.Graphics.GraphicsTypes;


export namespace Copium
{

    class Texture final
    {
    public:
        Texture(TextureDesc&& textureDesc, TextureData&& textureData);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& other) noexcept = default;
        Texture& operator=(Texture&& other) noexcept = default;

        [[nodiscard]] TextureHandle GetHandle() const { return m_textureHandle; }

    private:
        TextureDesc   m_textureDesc;
        TextureData   m_textureData;
        TextureHandle m_textureHandle;
    };

} // export namespace Copium
