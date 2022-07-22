module;

#include <string>

export module CopiumEngine.Assets.Texture;

import CopiumEngine.Core.Object;
import CopiumEngine.Graphics.GraphicsTypes;


export namespace Copium
{

    class Texture final : public Object
    {
    public:
        Texture(TextureDesc&& textureDesc);
        ~Texture();

        Texture(Texture&& other) noexcept = default;
        Texture& operator=(Texture&& other) noexcept = default;

        //< Object Interface
        [[nodiscard]] std::string GetName() const override { return m_textureDesc.Name; }
        //> Object Interface

        [[nodiscard]] TextureHandle GetHandle() const { return m_textureHandle; }

    private:
        TextureDesc   m_textureDesc;
        TextureHandle m_textureHandle;
    };

} // export namespace Copium
