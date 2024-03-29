module;

#include "Hesternal/Core/Debug.hpp"

module HesternalEngine.Graphics.GraphicsTypes;

import Hesternal.Core;

import <bit>;
import <utility>;


namespace
{
    using namespace Hesternal;


    // Minimal possible size
    static_assert(sizeof(GraphicsBufferDesc) == 12);
    static_assert(sizeof(SamplerDesc) == 12);

} // namespace


namespace Hesternal
{

    uint8 TextureFormat_BytesPerPixel(TextureFormat textureFormat) noexcept
    {
        static const uint8 bytesPerPixel[] = {
            1,
            1,
            1,
            1,
            2,
            2,
            2,
            2,
            4,
            4,
            4,
            4,
            2,
            2,
            2,
            2,
            2,
            4,
            4,
            4,
            4,
            4,
            8,
            8,
            8,
            8,
            8,
            4,
            4,
            4,
            8,
            8,
            8,
            16,
            16,
            16,
        };

        return bytesPerPixel[std::to_underlying(textureFormat)];
    }


    uint32 VertexAttributeDesc::Stride() const noexcept
    {
        static const uint32 vertexAttributeElementStride[] = {
            1, // SInt8
            2, // SInt16
            4, // SInt32
            1, // UInt8
            2, // UInt16
            4, // UInt32
            1, // SNorm8
            2, // SNorm16
            1, // UNorm8
            2, // UNorm16
            2, // Float16
            4, // Float32
        };

        return Dimension * vertexAttributeElementStride[std::to_underlying(Format)];
    }


    RenderTextureType RenderTextureDesc::RenderTextureType() const noexcept
    {
        if (Format == RenderTextureFormat::BGRA8)
        {
            return RenderTextureType::Color;
        }

        return RenderTextureType::Depth;
    }


    void TextureDesc::GenerateMipmaps(bool value)
    {
        if (value)
        {
            // NOTE(v.matushkin): https://stackoverflow.com/a/63987820/19262382
            //  Idk if this will handle all the cases
            MipmapCount = static_cast<uint8>(std::bit_width(std::max(Width, Height)));
        }
        else
        {
            MipmapCount = 1;
        }
    }


    void SamplerDesc::SetAnisotropicFilter(uint8 anisoLevel)
    {
        MinFilter = MagFilter = SamplerFilterMode::Linear;
        MipmapFilter = SamplerMipmapMode::Linear;

        if (anisoLevel > 1)
        {
            AnisoLevel = anisoLevel;
        }
        else
        {
            AnisoLevel = 0;
            HS_LOG_WARN("No reason to set anisoLevel < 2 ? Setting filter to Trilinear");
        }
    }

} // namespace Hesternal
