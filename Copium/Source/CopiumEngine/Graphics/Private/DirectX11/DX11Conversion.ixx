module;

#include "Engine/Core/Defines.hpp"

#include <d3d11_1.h>

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get 'import std;'
COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
COP_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
export module CopiumEngine.Graphics.DX11Conversion;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.GraphicsTypes;

import <utility>;
COP_WARNING_POP


export namespace Copium
{

    //- GraphicsBuffer
    [[nodiscard]] inline uint32 dx11_GraphicsBufferBindFlags(GraphicsBufferUsage graphicsBufferUsage) noexcept
    {
        static const D3D11_BIND_FLAG d3dBindFlags[] = {
            D3D11_BIND_VERTEX_BUFFER,
            D3D11_BIND_INDEX_BUFFER,
            D3D11_BIND_CONSTANT_BUFFER,
        };

        return d3dBindFlags[std::to_underlying(graphicsBufferUsage)];
    }

    //- Mesh
    [[nodiscard]] inline DXGI_FORMAT dx11_IndexFormat(IndexFormat indexFormat) noexcept
    {
        return indexFormat == IndexFormat::UInt16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    }

    //- RenderTexture
    #define DX11_NO_DEPTH_STENCIL 0
    [[nodiscard]] inline uint32 dx11_RenderTextureTypeToClearFlags(RenderTextureType renderTextureType) noexcept
    {
        static const uint32 d3dClearFlags[] = {
            DX11_NO_DEPTH_STENCIL,
            D3D11_CLEAR_DEPTH,
            // D3D11_CLEAR_STENCIL,
            // D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        };

        return d3dClearFlags[std::to_underlying(renderTextureType)];
    }

    //- Texture
    [[nodiscard]] inline D3D11_FILTER dx11_TextureFilterMode(TextureFilterMode textureFilterMode) noexcept
    {
        static const D3D11_FILTER d3dTextureFilterMode[] = {
            D3D11_FILTER_MIN_MAG_MIP_POINT,
            D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
            D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            D3D11_FILTER_ANISOTROPIC,
        };

        return d3dTextureFilterMode[std::to_underlying(textureFilterMode)];
    }

    [[nodiscard]] inline D3D11_TEXTURE_ADDRESS_MODE dx11_TextureWrapMode(TextureWrapMode textureWrapMode) noexcept
    {
        static const D3D11_TEXTURE_ADDRESS_MODE d3dTextureWrapMode[] = {
            D3D11_TEXTURE_ADDRESS_CLAMP,
            D3D11_TEXTURE_ADDRESS_BORDER,
            D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,
            D3D11_TEXTURE_ADDRESS_MIRROR,
            D3D11_TEXTURE_ADDRESS_WRAP,
        };

        return d3dTextureWrapMode[std::to_underlying(textureWrapMode)];
    }

    //- Shader states

    //-- RasterizerState
    [[nodiscard]] inline D3D11_CULL_MODE dx11_CullMode(CullMode cullMode) noexcept
    {
        static const D3D11_CULL_MODE d3dCullMode[] = {
            D3D11_CULL_NONE,
            D3D11_CULL_FRONT,
            D3D11_CULL_BACK,
        };

        return d3dCullMode[std::to_underlying(cullMode)];
    }

    [[nodiscard]] inline D3D11_FILL_MODE dx11_PolygonMode(PolygonMode polygonMode) noexcept
    {
        static const D3D11_FILL_MODE d3dPolygonMode[] = {
            D3D11_FILL_SOLID,
            D3D11_FILL_WIREFRAME,
        };

        return d3dPolygonMode[std::to_underlying(polygonMode)];
    }

    //-- DepthStencilState
    [[nodiscard]] inline D3D11_COMPARISON_FUNC dx11_CompareFunction(CompareFunction compareFunction) noexcept
    {
        static const D3D11_COMPARISON_FUNC d3dCompareFunction[] = {
            D3D11_COMPARISON_NEVER,
            D3D11_COMPARISON_LESS,
            D3D11_COMPARISON_EQUAL,
            D3D11_COMPARISON_LESS_EQUAL,
            D3D11_COMPARISON_GREATER,
            D3D11_COMPARISON_NOT_EQUAL,
            D3D11_COMPARISON_GREATER_EQUAL,
            D3D11_COMPARISON_ALWAYS,
        };

        return d3dCompareFunction[std::to_underlying(compareFunction)];
    }

    //-- BlendState
    [[nodiscard]] inline D3D11_BLEND_OP dx11_BlendOp(BlendOp blendOp) noexcept
    {
        static const D3D11_BLEND_OP d3dBlendOp[] = {
            D3D11_BLEND_OP_ADD,
            D3D11_BLEND_OP_SUBTRACT,
            D3D11_BLEND_OP_REV_SUBTRACT,
            D3D11_BLEND_OP_MIN,
            D3D11_BLEND_OP_MAX,
        };

        return d3dBlendOp[std::to_underlying(blendOp)];
    }

    [[nodiscard]] inline D3D11_BLEND dx11_BlendFactor(BlendFactor blendFactor) noexcept
    {
        static const D3D11_BLEND d3dBlendFactor[] = {
            D3D11_BLEND_ZERO,
            D3D11_BLEND_ONE,
            D3D11_BLEND_SRC_COLOR,
            D3D11_BLEND_INV_SRC_COLOR,
            D3D11_BLEND_DEST_COLOR,
            D3D11_BLEND_INV_DEST_COLOR,
            D3D11_BLEND_SRC_ALPHA,
            D3D11_BLEND_INV_SRC_ALPHA,
            D3D11_BLEND_DEST_ALPHA,
            D3D11_BLEND_INV_DEST_ALPHA,
            D3D11_BLEND_SRC_ALPHA_SAT,
            D3D11_BLEND_SRC1_COLOR,
            D3D11_BLEND_INV_SRC1_COLOR,
            D3D11_BLEND_SRC1_ALPHA,
            D3D11_BLEND_INV_SRC1_ALPHA,
        };

        return d3dBlendFactor[std::to_underlying(blendFactor)];
    }

    [[nodiscard]] inline D3D11_LOGIC_OP dx11_BlendLogicOp(BlendLogicOp blendLogicOp) noexcept
    {
        static const D3D11_LOGIC_OP d3dBlendLogicOp[] = {
            D3D11_LOGIC_OP_CLEAR,
            D3D11_LOGIC_OP_SET,
            D3D11_LOGIC_OP_COPY,
            D3D11_LOGIC_OP_COPY_INVERTED,
            D3D11_LOGIC_OP_NOOP,
            D3D11_LOGIC_OP_INVERT,
            D3D11_LOGIC_OP_AND,
            D3D11_LOGIC_OP_NAND,
            D3D11_LOGIC_OP_OR,
            D3D11_LOGIC_OP_NOR,
            D3D11_LOGIC_OP_XOR,
            D3D11_LOGIC_OP_EQUIV,
            D3D11_LOGIC_OP_AND_REVERSE,
            D3D11_LOGIC_OP_AND_INVERTED,
            D3D11_LOGIC_OP_OR_REVERSE,
            D3D11_LOGIC_OP_OR_INVERTED,
        };

        return d3dBlendLogicOp[std::to_underlying(blendLogicOp)];
    }

} // export namespace Copium
