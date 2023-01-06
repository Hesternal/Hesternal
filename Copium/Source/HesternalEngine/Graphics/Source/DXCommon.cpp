module;

#include "Copium/Core/Debug.hpp"

#include <dxgiformat.h>

module HesternalEngine.Graphics.DXCommon;

import Hesternal.Core;

import HesternalEngine.Graphics.GraphicsTypes;

import <utility>;


namespace Hesternal
{

    DXGI_FORMAT dx_RenderTextureFormat(RenderTextureFormat renderTextureFormat)
    {
        static constinit const DXGI_FORMAT dxgiRenderTextureFormat[] = {
            DXGI_FORMAT_B8G8R8A8_UNORM,
            DXGI_FORMAT_D32_FLOAT,
        };

        return dxgiRenderTextureFormat[std::to_underlying(renderTextureFormat)];
    }

    // NOTE(v.matushkin): What about BGRA/RGBA ?
    DXGI_FORMAT dx_TextureFormat(TextureFormat textureFormat)
    {
        static constinit const DXGI_FORMAT dxgiTextureFormat[] = {
            DXGI_FORMAT_R8_UNORM,
            DXGI_FORMAT_R8_SNORM,
            DXGI_FORMAT_R8_UINT,
            DXGI_FORMAT_R8_SINT,
            DXGI_FORMAT_R8G8_UNORM,
            DXGI_FORMAT_R8G8_SNORM,
            DXGI_FORMAT_R8G8_UINT,
            DXGI_FORMAT_R8G8_SINT,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_FORMAT_R8G8B8A8_SNORM,
            DXGI_FORMAT_R8G8B8A8_UINT,
            DXGI_FORMAT_R8G8B8A8_SINT,
            DXGI_FORMAT_R16_UNORM,
            DXGI_FORMAT_R16_SNORM,
            DXGI_FORMAT_R16_UINT,
            DXGI_FORMAT_R16_SINT,
            DXGI_FORMAT_R16_FLOAT,
            DXGI_FORMAT_R16G16_UNORM,
            DXGI_FORMAT_R16G16_SNORM,
            DXGI_FORMAT_R16G16_UINT,
            DXGI_FORMAT_R16G16_SINT,
            DXGI_FORMAT_R16G16_FLOAT,
            DXGI_FORMAT_R16G16B16A16_UNORM,
            DXGI_FORMAT_R16G16B16A16_SNORM,
            DXGI_FORMAT_R16G16B16A16_UINT,
            DXGI_FORMAT_R16G16B16A16_SINT,
            DXGI_FORMAT_R16G16B16A16_FLOAT,
            DXGI_FORMAT_R32_UINT,
            DXGI_FORMAT_R32_SINT,
            DXGI_FORMAT_R32_FLOAT,
            DXGI_FORMAT_R32G32_UINT,
            DXGI_FORMAT_R32G32_SINT,
            DXGI_FORMAT_R32G32_FLOAT,
            DXGI_FORMAT_R32G32B32A32_UINT,
            DXGI_FORMAT_R32G32B32A32_SINT,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
        };

        return dxgiTextureFormat[std::to_underlying(textureFormat)];
    }

    DXGI_FORMAT dx_VertexAttributeFormat(VertexAttributeFormat format, uint8 dimension)
    {
        static constinit const DXGI_FORMAT dxgiVertexFormat[] = {
            // SInt8
            DXGI_FORMAT_R8_SINT,
            DXGI_FORMAT_R8G8_SINT,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_R8G8B8A8_SINT,
            // SInt16
            DXGI_FORMAT_R16_SINT,
            DXGI_FORMAT_R16G16_SINT,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_R16G16B16A16_SINT,
            // SInt32
            DXGI_FORMAT_R32_SINT,
            DXGI_FORMAT_R32G32_SINT,
            DXGI_FORMAT_R32G32B32_SINT,
            DXGI_FORMAT_R32G32B32A32_SINT,
            // UInt8
            DXGI_FORMAT_R8_UINT,
            DXGI_FORMAT_R8G8_UINT,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_R8G8B8A8_UINT,
            // UInt16
            DXGI_FORMAT_R16_UINT,
            DXGI_FORMAT_R16G16_UINT,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_R16G16B16A16_UINT,
            // UInt32
            DXGI_FORMAT_R32_UINT,
            DXGI_FORMAT_R32G32_UINT,
            DXGI_FORMAT_R32G32B32_UINT,
            DXGI_FORMAT_R32G32B32A32_UINT,
            // SNorm8
            DXGI_FORMAT_R8_SNORM,
            DXGI_FORMAT_R8G8_SNORM,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_R8G8B8A8_SNORM,
            // SNorm16
            DXGI_FORMAT_R16_SNORM,
            DXGI_FORMAT_R16G16_SNORM,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_R16G16B16A16_SNORM,
            // UNorm8
            DXGI_FORMAT_R8_UNORM,
            DXGI_FORMAT_R8G8_UNORM,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            // UNorm16
            DXGI_FORMAT_R16_UNORM,
            DXGI_FORMAT_R16G16_UNORM,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_R16G16B16A16_UNORM,
            // Float16
            DXGI_FORMAT_R16_FLOAT,
            DXGI_FORMAT_R16G16_FLOAT,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_R16G16B16A16_FLOAT,
            // Float32
            DXGI_FORMAT_R32_FLOAT,
            DXGI_FORMAT_R32G32_FLOAT,
            DXGI_FORMAT_R32G32B32_FLOAT,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
        };

        COP_ASSERT_MSG(dimension >= 1 && dimension <= 4, "Vertex attribute dimension must be in [1,4] range");
        const DXGI_FORMAT dxgiFormat = dxgiVertexFormat[std::to_underlying(format) * 4 + dimension - 1];
        COP_ASSERT_MSG(dxgiFormat != DXGI_FORMAT_UNKNOWN, "Unsupported vertex format/dimension combination");

        return dxgiFormat;
    }

} // namespace Hesternal
