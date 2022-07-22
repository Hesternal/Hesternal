module;

#include <string>

export module CopiumEditor.Assets.TextureImporter;

import CopiumEngine.Graphics.GraphicsTypes;


export namespace Copium
{

    class TextureImporter final
    {
    public:
        [[nodiscard]] static TextureDesc Import(const std::string& texturePath);
    };

} // export namespace Copium
