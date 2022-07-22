module;

#include "Engine/Core/Defines.hpp"
#include "EngineCht/GraphicsTypes.cht.hpp"

#include <memory>
#include <string>
#include <vector>

export module CopiumEngine.Graphics.GraphicsTypes;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Core.Serialization.IBinaryConverter;


namespace Copium
{
    inline constexpr uint32 k_InvalidHandle = 0xffff'ffff;
}


export namespace Copium
{

    enum class MeshHandle      : uint32 { Invalid = k_InvalidHandle };
    enum class ShaderHandle    : uint32 { Invalid = k_InvalidHandle };
    enum class SwapchainHandle : uint32 { Invalid = k_InvalidHandle };
    enum class TextureHandle   : uint32 { Invalid = k_InvalidHandle };


    // TODO(v.matushkin): Add Default color/depth formats that depends on the current platform
    enum class RenderTextureFormat : uint8
    {
        BGRA32,
        Depth32,
    };

    enum class BlendFactor : uint8
    {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ScrAlphaSaturate,
        // TODO(v.matushkin): How this '*1*' values work?
        Src1Color,
        OneMinusSrc1Color,
        Src1Alpha,
        OneMinusSrc1Alpha,
    };

    enum class TextureFormat : uint8
    {
        R8,
        RG8,
        RGBA8,
    };

    enum class TextureWrapMode : uint8
    {
        ClampToEdge,
        ClampToBorder,
        MirroredOnce,
        MirroredRepeat,
        Repeat,
    };

    enum class IndexFormat : uint8
    {
        UInt16,
        UInt32,
    };

    enum class VertexAttribute : uint8
    {
        Position,
        Normal,
        UV0,
    };

    // NOTE(v.matushkin): Seems like OpenGL/Vulkan support more formats (VK_FORMAT_R64_SFLOAT for example)
    enum class VertexAttributeFormat : uint8
    {
        SInt8,
        SInt16,
        SInt32,
        UInt8,
        UInt16,
        UInt32,
        SNorm8,
        SNorm16,
        UNorm8,
        UNorm16,
        Float16,
        Float32,
    };


    CHT_STRUCT()
    struct VertexAttributeDesc
    {
        CHT_GENERATED_BODY()

        CHT_PROPERTY()
        uint32                Offset;
        CHT_PROPERTY()
        uint8                 Dimension;
        CHT_PROPERTY()
        uint8                 Stream;
        CHT_PROPERTY()
        VertexAttribute       Attribute; // NOTE(v.matushkin): Do I need this?
        CHT_PROPERTY()
        VertexAttributeFormat Format;
    };

    CHT_STRUCT()
    struct MeshDesc
    {
        CHT_GENERATED_BODY()

        CHT_PROPERTY()
        std::string         Name;
        CHT_PROPERTY()
        VertexAttributeDesc Position;
        CHT_PROPERTY()
        VertexAttributeDesc Normal;
        CHT_PROPERTY()
        VertexAttributeDesc UV0;
        CHT_PROPERTY()
        uint32              IndexCount;
        CHT_PROPERTY()
        uint32              VertexCount;
        CHT_PROPERTY()
        std::vector<uint8>  IndexData;
        CHT_PROPERTY()
        std::vector<uint8>  VertexData;
    };

    CHT_STRUCT()
    struct TextureDesc
    {
        CHT_GENERATED_BODY()

        CHT_PROPERTY()
        std::string        Name;
        CHT_PROPERTY()
        uint32             Width;
        CHT_PROPERTY()
        uint32             Height;
        CHT_PROPERTY()
        TextureFormat      Format;
        CHT_PROPERTY()
        TextureWrapMode    WrapModeU;
        CHT_PROPERTY()
        TextureWrapMode    WrapModeV;
        CHT_PROPERTY()
        std::vector<uint8> Data;
    };

    struct ShaderDesc
    {
        std::string Name;
        std::string VertexSource;
        std::string FragmentSource;
    };

    struct SwapchainDesc
    {
        uint64              WindowNativeHandle;
        uint16              Width;
        uint16              Height;
        uint8               BufferCount;
        RenderTextureFormat Format;
    };

} // export namespace Copium
