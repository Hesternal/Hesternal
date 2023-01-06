export module HesternalEditor.Assets.TextureImporter;

import HesternalEngine.Assets.Texture;

import <string>;


export namespace Hesternal
{

    class TextureImporter final
    {
    public:
        [[nodiscard]] static Texture Import(const std::string& texturePath);
    };

} // export namespace Hesternal
