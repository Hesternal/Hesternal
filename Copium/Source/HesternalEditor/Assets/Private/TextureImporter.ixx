export module CopiumEditor.Assets.TextureImporter;

import CopiumEngine.Assets.Texture;

import <string>;


export namespace Copium
{

    class TextureImporter final
    {
    public:
        [[nodiscard]] static Texture Import(const std::string& texturePath);
    };

} // export namespace Copium
