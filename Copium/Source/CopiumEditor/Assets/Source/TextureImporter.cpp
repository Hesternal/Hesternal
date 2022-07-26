module;

#include "Engine/Core/Debug.hpp"

// NOTE(v.matushkin): Put stb in a normal conan package? not this fucking trash that I get rn
// NOTE(v.matushkin): It's kinda dumb that stb error checking depends on 'COP_ENABLE_ASSERTS'
#if COP_ENABLE_ASSERTS
#define STBI_FAILURE_USERMSG
#else
#define STBI_NO_FAILURE_STRINGS
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#ifdef COP_ENABLE_ASSERTS
#undef STBI_FAILURE_USERMSG
#else
#undef STBI_NO_FAILURE_STRINGS
#endif

module CopiumEditor.Assets.TextureImporter;

import CopiumEngine.Core.CoreTypes;

import <string>;
import <utility>;
import <vector>;


// NOTE(v.matushkin): There is STBI_WINDOWS_UTF8 and stbi_convert_wchar_to_utf8


namespace
{

    using namespace Copium;


    [[nodiscard]] static TextureFormat NumComponentsToTextureFormat(int32 numComponents)
    {
        if (numComponents == 1)
        {
            return TextureFormat::R8;
        }
        if (numComponents == 2)
        {
            return TextureFormat::RG8;
        }
        return TextureFormat::RGBA8;
    }

} // namespace


namespace Copium
{

    Texture TextureImporter::Import(const std::string& texturePath)
    {
        stbi_set_flip_vertically_on_load(true); // TODO(v.matushkin): Set only once

        int32 width, height, numComponents;
        // TODO(v.matushkin): Check for errors
        stbi_info(texturePath.c_str(), &width, &height, &numComponents);

        const int32 desiredComponents = numComponents == 3 ? 4 : numComponents;

        uint8* stbImageData = stbi_load(texturePath.c_str(), &width, &height, &numComponents, desiredComponents);
        COP_ASSERT_MSG(stbImageData != nullptr, stbi_failure_reason());

        const int32 textureSize = width * height * desiredComponents;
        TextureDesc textureDesc = {
            .Name      = texturePath, // TODO(v.matushkin): Name, not path
            .Width     = static_cast<uint32>(width),
            .Height    = static_cast<uint32>(height),
            .Format    = NumComponentsToTextureFormat(desiredComponents),
            .WrapModeU = TextureWrapMode::Repeat,
            .WrapModeV = TextureWrapMode::Repeat,
            .Data      = std::vector<uint8>(stbImageData, stbImageData + textureSize),
        };

        stbi_image_free(stbImageData);

        return Texture(std::move(textureDesc));
    }

} // namespace Copium
