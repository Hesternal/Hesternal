module;

#include "Engine/Core/Defines.hpp"
#include "EngineCht/GraphicsTypes.cht.hpp"

#include <memory>
#include <string>
#include <vector>

export module CopiumEngine.Graphics.GraphicsTypes;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Core.ForwardDeclaration;


namespace Copium
{
    inline constexpr uint32 k_InvalidHandle = 0xffff'ffff;
}


export namespace Copium
{

    enum class MeshHandle          : uint32 { Invalid = k_InvalidHandle };
    enum class RenderTextureHandle : uint32 { Invalid = k_InvalidHandle };
    enum class ShaderHandle        : uint32 { Invalid = k_InvalidHandle };
    enum class SwapchainHandle     : uint32 { Invalid = k_InvalidHandle };
    enum class TextureHandle       : uint32 { Invalid = k_InvalidHandle };


    //- Shader states

    //-- RasterizerState
    enum class PolygonMode : uint8
    {
        Fill,
        Wireframe,
    };

    enum class CullMode : uint8
    {
        Off,
        Front,
        Back,
        // OpenGl and Vulkan have FrontAndBack
    };

    enum class TriangleFrontFace : uint8
    {
        Clockwise,
        CounterClockwise,
    };

    //-- DepthStencilState
    enum class DepthTest : uint8
    {
        Off,
        ReadOnly,
        ReadWrite,
    };

    enum class CompareFunction : uint8
    {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always,
    };

    //-- BlendState
    enum class BlendMode : uint8
    {
        Off,
        BlendOp,
        LogicOp,
    };

    enum class BlendOp : uint8
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max,
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

    enum class BlendLogicOp : uint8
    {
        Clear,
        Set,
        Copy,
        CopyInversed,
        Noop,
        Invert,
        And,
        Nand,
        Or,
        Nor,
        Xor,
        Equivalent,
        AndReverse,
        AndInverted,
        OrReverse,
        OrInverted,
    };


    //- RenderTexture

    // TODO(v.matushkin): Add Default color/depth formats that depends on the current platform
    enum class RenderTextureFormat : uint8
    {
        BGRA8,
        Depth32,
    };

    // NOTE(v.matushkin): Not sure about this enum
    enum class RenderTextureType : uint8
    {
        Color,
        Depth,
        Stencil,
        DepthStencil,
    };

    // NOTE(v.matushkin): Not sure about this enum and the naming
    enum class RenderTextureUsage : uint8
    {
        Default,
        ShaderRead,
    };


    //- Texture

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


    //- Mesh

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
    // Edit VertexAttributeDesc::SizeInBytes() if changing something here
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
        uint8                 Offset; // NOTE(v.matushkin): May be stride is a better name than offset?
        CHT_PROPERTY()
        uint8                 Dimension;
        CHT_PROPERTY()
        uint8                 Stream;
        CHT_PROPERTY()
        VertexAttribute       Attribute; // NOTE(v.matushkin): Do I need this?
        CHT_PROPERTY()
        VertexAttributeFormat Format;

        [[nodiscard]] uint32 Stride() const;

        [[nodiscard]] static VertexAttributeDesc Position();
        [[nodiscard]] static VertexAttributeDesc Normal();
        [[nodiscard]] static VertexAttributeDesc UV0();
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
        IndexFormat         IndexFormat;
        CHT_PROPERTY()
        uint32              IndexCount;
        CHT_PROPERTY()
        uint32              VertexCount;
        CHT_PROPERTY()
        std::vector<uint8>  IndexData;
        CHT_PROPERTY()
        std::vector<uint8>  VertexData;
    };

    struct ClearColorValue
    {
        float32 Value[4];

        [[nodiscard]] static ClearColorValue Default();
    };

    struct ClearDepthStencilValue
    {
        float32 Depth;
        uint8   Stencil;

        [[nodiscard]] static ClearDepthStencilValue Default();
    };

    union RenderTextureClearValue
    {
        ClearColorValue        Color;
        ClearDepthStencilValue DepthStencil;

        [[nodiscard]] static RenderTextureClearValue DefaultColor();
        [[nodiscard]] static RenderTextureClearValue DefaultDepthStencil();
    };

    struct RenderTextureDesc
    {
        std::string             Name;
        RenderTextureClearValue ClearValue;
        uint32                  Width;
        uint32                  Height;
        RenderTextureFormat     Format;
        RenderTextureUsage      Usage;

        [[nodiscard]] RenderTextureType RenderTextureType() const;
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

    struct RasterizerStateDesc
    {
        PolygonMode       PolygonMode;
        CullMode          CullMode;
        TriangleFrontFace FrontFace;

        [[nodiscard]] static RasterizerStateDesc Default();
    };

    struct DepthStencilStateDesc
    {
        bool            DepthTestEnable;
        bool            DepthWriteEnable;
        CompareFunction DepthCompareFunction;

        [[nodiscard]] static DepthStencilStateDesc Default();
    };

    struct BlendStateDesc
    {
        BlendMode    BlendMode;
        BlendFactor  ColorSrcBlendFactor;
        BlendFactor  ColorDstBlendFactor;
        BlendOp      ColorBlendOp;
        BlendFactor  AlphaSrcBlendFactor;
        BlendFactor  AlphaDstBlendFactor;
        BlendOp      AlphaBlendOp;
        BlendLogicOp LogicOp;

        [[nodiscard]] static BlendStateDesc Default();
    };

    CHT_STRUCT()
    struct ShaderDesc
    {
        CHT_GENERATED_BODY()

        CHT_PROPERTY()
        std::string Name;

        RasterizerStateDesc   RasterizerStateDesc;
        DepthStencilStateDesc DepthStencilStateDesc;
        BlendStateDesc        BlendStateDesc;

        CHT_PROPERTY()
        std::string VertexSource;
        CHT_PROPERTY()
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
