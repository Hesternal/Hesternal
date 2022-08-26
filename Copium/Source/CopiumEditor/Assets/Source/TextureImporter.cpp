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

import <bit>;
import <cstring>;
import <memory>;
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
            return TextureFormat::R8_UNorm;
        }
        if (numComponents == 2)
        {
            return TextureFormat::RG8_UNorm;
        }
        return TextureFormat::RGBA8_UNorm;
    }


    // TODO(v.matushkin): What about NormalMap mips (renormalize?) and Alpha channel used for transparency(PreserveCoverage?)
    // TODO(v.matushkin): Mipmaps generated with a simple Box filter.
    //  Implement different filters?
    //  This implementation can also be improved in terms of speed and quality,
    //   eg. generating Mip[1..n] from Mip0 should improve quality,
    //   and may be there is a way to use SIMD instructions.
    // TODO(v.matushkin): I allocate memory for the whole texture, copy Mip0 into it and generate mipmaps, all in one function called GenerateMips,
    //  probably it does a lot more than it should, especially if I don't want to generate mips.
    [[nodiscard]] static void GenerateMips(const uint8* imageData, TextureDesc& textureDesc)
    {
        const uint8 bytesPerPixel = TextureFormat_BytesPerPixel(textureDesc.Format);

        //- Get mipmap data sizes
        uint32 textureDataSize = 0;
        auto mipDataSizes = std::make_unique<uint32[]>(textureDesc.MipmapCount);

        {
            uint32 width = textureDesc.Width;
            uint32 height = textureDesc.Height;

            for (uint8 mipLevel = 0; mipLevel < textureDesc.MipmapCount; mipLevel++)
            {
                const uint32 mipDataSize = width * height * bytesPerPixel;
                mipDataSizes[mipLevel] = mipDataSize;
                textureDataSize += mipDataSize;
                width >>= 1;
                height >>= 1;

                if (width == 0) width = 1;
                if (height == 0) height = 1;
            }
        }

        textureDesc.Data = std::vector<uint8>(textureDataSize);
        uint8* textureDataPtr = textureDesc.Data.data();

        //- Copy Mip0
        std::memcpy(textureDataPtr, imageData, mipDataSizes[0]);

        //- Generate mipmaps with Box filter
        uint8* previousMipDataPtr = textureDataPtr;
        textureDataPtr += mipDataSizes[0];

        uint32 previousMipWidth = textureDesc.Width;
        uint32 currentMipWidth = textureDesc.Width >> 1;
        uint32 currentMipHeight = textureDesc.Height >> 1;

        for (uint8 mipLevel = 1; mipLevel < textureDesc.MipmapCount; mipLevel++)
        {
            for (uint32 y = 0; y < currentMipHeight; y++)
            {
                const uint32 previousMipRowSize = previousMipWidth * bytesPerPixel;
                const uint32 previousMipRow0 = (y * 2)     * previousMipRowSize;
                const uint32 previousMipRow1 = (y * 2 + 1) * previousMipRowSize;

                for (uint32 x = 0; x < currentMipWidth; x++)
                {
                    const uint32 previousMipColumn0 = (x * 2)     * bytesPerPixel;
                    const uint32 previousMipColumn1 = (x * 2 + 1) * bytesPerPixel;

                    for (uint8 byte = 0; byte < bytesPerPixel; byte++)
                    {
                        uint32 texel00 = previousMipDataPtr[previousMipRow0 + previousMipColumn0 + byte];
                        uint32 texel01 = previousMipDataPtr[previousMipRow0 + previousMipColumn1 + byte];
                        uint32 texel10 = previousMipDataPtr[previousMipRow1 + previousMipColumn0 + byte];
                        uint32 texel11 = previousMipDataPtr[previousMipRow1 + previousMipColumn1 + byte];

                        uint8 result = static_cast<uint8>((texel00 + texel10 + texel01 + texel11) / 4);

                        textureDataPtr[y * currentMipWidth * bytesPerPixel + x * bytesPerPixel + byte] = result;
                    }
                }
            }

            previousMipDataPtr = textureDataPtr;
            textureDataPtr += mipDataSizes[mipLevel];

            previousMipWidth = currentMipWidth;
            currentMipWidth >>= 1;
            currentMipHeight >>= 1;

            if (currentMipWidth == 0) currentMipWidth = 1;
            if (currentMipHeight == 0) currentMipHeight = 1;
        }
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

        uint32 widthUnsigned = static_cast<uint32>(width);
        uint32 heightUnsigned = static_cast<uint32>(height);
        COP_ASSERT_MSG(std::has_single_bit(widthUnsigned) && std::has_single_bit(heightUnsigned), "One of the texture dimensions is not a power of 2");

        const int32 desiredComponents = numComponents == 3 ? 4 : numComponents;

        uint8* stbImageData = stbi_load(texturePath.c_str(), &width, &height, &numComponents, desiredComponents);
        COP_ASSERT_MSG(stbImageData != nullptr, stbi_failure_reason());

        TextureDesc textureDesc;
        textureDesc.Name   = texturePath, // TODO(v.matushkin): Name, not path
        textureDesc.Width  = widthUnsigned;
        textureDesc.Height = heightUnsigned;
        textureDesc.Format = NumComponentsToTextureFormat(desiredComponents);
        textureDesc.GenerateMipmaps(true);
        textureDesc.SetAnisotropicFilterMode(16);
        textureDesc.SetWrapMode(TextureWrapMode::Repeat);

        GenerateMips(stbImageData, textureDesc);

        stbi_image_free(stbImageData);

        return Texture(std::move(textureDesc));
    }

} // namespace Copium
