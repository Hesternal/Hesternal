module;

#include <dxgiformat.h>

export module CopiumEngine.Graphics.DXCommon;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.GraphicsTypes;


export namespace Copium
{
    //- Texture
    [[nodiscard]] DXGI_FORMAT dx_RenderTextureFormat(RenderTextureFormat renderTextureFormat);
    [[nodiscard]] DXGI_FORMAT dx_TextureFormat(TextureFormat textureFormat);

    [[nodiscard]] DXGI_FORMAT dx_VertexAttributeFormat(VertexAttributeFormat format, uint8 dimension);

    /// For DirectX Clockwise - false, CounterClockwise - true
    [[nodiscard]] bool dx_TriangleFrontFace(TriangleFrontFace triangleFrontFace);

} // export namespace Copium
