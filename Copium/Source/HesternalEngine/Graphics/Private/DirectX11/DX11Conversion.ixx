module;

#include "Copium/Core/Defines.hpp"

#include <d3d11_1.h>

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get 'import std;'
COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
COP_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
export module HesternalEngine.Graphics.DX11Conversion;

import Hesternal.Core;

import HesternalEngine.Graphics.GraphicsTypes;

import <utility>;
COP_WARNING_POP


// https://developercommunity.visualstudio.com/t/VS-2022-1750-Preview-2-C20-modules-b/10229203
#define _COP_STATIC_ARRAY_BUG_FIXED false

// NOTE(v.matushkin): I should mark this functions as `constexpr` anyway, but then I can't mark array as `static`
//   and I don't know how static array works in constexpr function.

#if _COP_STATIC_ARRAY_BUG_FIXED
    #define _COP_STATIC_ARRAY static constinit
    #define _COP_FUNCTION     inline
#else
    #define _COP_STATIC_ARRAY constexpr
    #define _COP_FUNCTION     constexpr
#endif


export namespace Hesternal
{

    //- GraphicsBuffer
    [[nodiscard]] _COP_FUNCTION uint32 dx11_GraphicsBufferBindFlags(GraphicsBufferUsage graphicsBufferUsage) noexcept
    {
        _COP_STATIC_ARRAY D3D11_BIND_FLAG d3dBindFlags[] = {
            D3D11_BIND_VERTEX_BUFFER,
            D3D11_BIND_INDEX_BUFFER,
            D3D11_BIND_CONSTANT_BUFFER,
        };

        return d3dBindFlags[std::to_underlying(graphicsBufferUsage)];
    }

    //- Mesh
    [[nodiscard]] constexpr DXGI_FORMAT dx11_IndexFormat(IndexFormat indexFormat) noexcept
    {
        return indexFormat == IndexFormat::UInt16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    }

    //- RenderTexture
    #define DX11_NO_DEPTH_STENCIL 0
    [[nodiscard]] _COP_FUNCTION uint32 dx11_RenderTextureTypeToClearFlags(RenderTextureType renderTextureType) noexcept
    {
        _COP_STATIC_ARRAY uint32 d3dClearFlags[] = {
            DX11_NO_DEPTH_STENCIL,
            D3D11_CLEAR_DEPTH,
            // D3D11_CLEAR_STENCIL,
            // D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        };

        return d3dClearFlags[std::to_underlying(renderTextureType)];
    }

    //- Sampler
    [[nodiscard]] constexpr D3D11_FILTER dx11_SamplerFilter(SamplerFilterMode minFilter, SamplerFilterMode magFilter, SamplerMipmapMode mipmapMode, uint8 anisoLevel) noexcept
    {
        D3D11_FILTER_TYPE d3dMinFilter = minFilter == SamplerFilterMode::Nearest ? D3D11_FILTER_TYPE_POINT : D3D11_FILTER_TYPE_LINEAR;
        D3D11_FILTER_TYPE d3dMagFilter = magFilter == SamplerFilterMode::Nearest ? D3D11_FILTER_TYPE_POINT : D3D11_FILTER_TYPE_LINEAR;
        D3D11_FILTER_TYPE d3dMipmapFilter = mipmapMode == SamplerMipmapMode::Nearest ? D3D11_FILTER_TYPE_POINT : D3D11_FILTER_TYPE_LINEAR;

        uint32 d3dSamplerFilter = d3dMinFilter << D3D11_MIN_FILTER_SHIFT
                                | d3dMagFilter << D3D11_MAG_FILTER_SHIFT
                                | d3dMipmapFilter << D3D11_MIP_FILTER_SHIFT
                                | (anisoLevel == 0 ? 0 : D3D11_FILTER_ANISOTROPIC);

        return static_cast<D3D11_FILTER>(d3dSamplerFilter);
    }

    [[nodiscard]] _COP_FUNCTION D3D11_TEXTURE_ADDRESS_MODE dx11_SamplerAddressMode(SamplerAddressMode samplerAddressMode) noexcept
    {
        _COP_STATIC_ARRAY D3D11_TEXTURE_ADDRESS_MODE d3dSamplerAddressMode[] = {
            D3D11_TEXTURE_ADDRESS_CLAMP,
            D3D11_TEXTURE_ADDRESS_BORDER,
            D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,
            D3D11_TEXTURE_ADDRESS_MIRROR,
            D3D11_TEXTURE_ADDRESS_WRAP,
        };

        return d3dSamplerAddressMode[std::to_underlying(samplerAddressMode)];
    }

    //- Shader states

    //-- RasterizerState
    [[nodiscard]] _COP_FUNCTION D3D11_CULL_MODE dx11_CullMode(CullMode cullMode) noexcept
    {
        _COP_STATIC_ARRAY D3D11_CULL_MODE d3dCullMode[] = {
            D3D11_CULL_NONE,
            D3D11_CULL_FRONT,
            D3D11_CULL_BACK,
        };

        return d3dCullMode[std::to_underlying(cullMode)];
    }

    [[nodiscard]] _COP_FUNCTION D3D11_FILL_MODE dx11_PolygonMode(PolygonMode polygonMode) noexcept
    {
        _COP_STATIC_ARRAY D3D11_FILL_MODE d3dPolygonMode[] = {
            D3D11_FILL_SOLID,
            D3D11_FILL_WIREFRAME,
        };

        return d3dPolygonMode[std::to_underlying(polygonMode)];
    }

    //-- DepthStencilState
    [[nodiscard]] _COP_FUNCTION D3D11_COMPARISON_FUNC dx11_CompareFunction(CompareFunction compareFunction) noexcept
    {
        _COP_STATIC_ARRAY D3D11_COMPARISON_FUNC d3dCompareFunction[] = {
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
    [[nodiscard]] _COP_FUNCTION D3D11_BLEND_OP dx11_BlendOp(BlendOp blendOp) noexcept
    {
        _COP_STATIC_ARRAY D3D11_BLEND_OP d3dBlendOp[] = {
            D3D11_BLEND_OP_ADD,
            D3D11_BLEND_OP_SUBTRACT,
            D3D11_BLEND_OP_REV_SUBTRACT,
            D3D11_BLEND_OP_MIN,
            D3D11_BLEND_OP_MAX,
        };

        return d3dBlendOp[std::to_underlying(blendOp)];
    }

    [[nodiscard]] _COP_FUNCTION D3D11_BLEND dx11_BlendFactor(BlendFactor blendFactor) noexcept
    {
        _COP_STATIC_ARRAY D3D11_BLEND d3dBlendFactor[] = {
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

    [[nodiscard]] _COP_FUNCTION D3D11_LOGIC_OP dx11_BlendLogicOp(BlendLogicOp blendLogicOp) noexcept
    {
        _COP_STATIC_ARRAY D3D11_LOGIC_OP d3dBlendLogicOp[] = {
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

} // export namespace Hesternal
