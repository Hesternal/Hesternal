export module CopiumEngine.Assets.Texture;

import CopiumEngine.Graphics.GraphicsTypes;


export namespace Copium
{

    class Texture final
    {
    public:
        Texture(TextureDesc&& textureDesc, TextureData&& textureData);
        ~Texture();

        [[nodiscard]] TextureHandle GetHandle() const { return m_textureHandle; }

    private:
        TextureDesc   m_textureDesc;
        TextureData   m_textureData;
        TextureHandle m_textureHandle;
    };

} // export namespace Copium
