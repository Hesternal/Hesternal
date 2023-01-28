module;

#include "Hesternal/Core/Defines.hpp"

#include <dxgiformat.h>

export module HesternalEngine.Graphics.DXCommon;

import Hesternal.Core;

import HesternalEngine.Graphics.GraphicsTypes;


export namespace Hesternal
{

    //- Texture
    [[nodiscard]] DXGI_FORMAT dx_RenderTextureFormat(RenderTextureFormat renderTextureFormat);
    [[nodiscard]] DXGI_FORMAT dx_TextureFormat(TextureFormat textureFormat);

    [[nodiscard]] DXGI_FORMAT dx_VertexAttributeFormat(VertexAttributeFormat format, uint8 dimension);

    /// For DirectX Clockwise - false, CounterClockwise - true
    [[nodiscard]] HS_FORCE_INLINE constexpr bool dx_TriangleFrontFace(TriangleFrontFace triangleFrontFace) noexcept
    {
        return triangleFrontFace == TriangleFrontFace::CounterClockwise;
    }

} // export namespace Hesternal
