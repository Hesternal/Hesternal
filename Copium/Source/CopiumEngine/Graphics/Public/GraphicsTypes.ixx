module;

#include "Copium/Core/Defines.hpp"
#include "EngineCht/GraphicsTypes.cht.hpp"

export module CopiumEngine.Graphics.GraphicsTypes;

import Copium.Core;

import CopiumEngine.Core.ForwardDeclaration;

import <memory>;
import <optional>;
import <string>;
import <vector>;


namespace
{
    using namespace Copium;


    inline constexpr uint32 k_InvalidHandle = 0xffff'ffff;

} // namespace


export namespace Copium
{

    enum class GraphicsBufferHandle : uint32 { Invalid = k_InvalidHandle };
    enum class MeshHandle           : uint32 { Invalid = k_InvalidHandle };
    enum class RenderPassHandle     : uint32 { Invalid = k_InvalidHandle };
    enum class RenderTextureHandle  : uint32 { Invalid = k_InvalidHandle };
    enum class ShaderHandle         : uint32 { Invalid = k_InvalidHandle };
    enum class SwapchainHandle      : uint32 { Invalid = k_InvalidHandle };
    enum class TextureHandle        : uint32 { Invalid = k_InvalidHandle };


    // ========================================================================
    // ============================ GraphicsBuffer ============================
    // ========================================================================

    enum class GraphicsBufferUsage : uint8
    {
        Vertex,
        Index,
        Constant,
    };

    struct GraphicsBufferDesc final
    {
        uint32              ElementCount;
        uint32              ElementSize;
        GraphicsBufferUsage Usage;

        [[nodiscard]] uint32 SizeInBytes() const noexcept { return ElementCount * ElementSize; }

        [[nodiscard]] static GraphicsBufferDesc Vertex(uint32 elementCount, uint32 elementSize) noexcept;
        [[nodiscard]] static GraphicsBufferDesc Index(uint32 elementCount, uint32 elementSize) noexcept;
        [[nodiscard]] static GraphicsBufferDesc Constant(uint32 elementCount, uint32 elementSize) noexcept;
    };


    // ========================================================================
    // ================================= Mesh =================================
    // ========================================================================

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
    struct VertexAttributeDesc final
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

        [[nodiscard]] uint32 Stride() const noexcept;

        [[nodiscard]] static VertexAttributeDesc Position() noexcept;
        [[nodiscard]] static VertexAttributeDesc Normal() noexcept;
        [[nodiscard]] static VertexAttributeDesc UV0() noexcept;
    };

    CHT_STRUCT()
    struct MeshDesc final
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


    // ========================================================================
    // =============================== Texture ================================
    // ========================================================================

    enum class TextureFormat : uint8
    {
        R8_UNorm,
        R8_SNorm,
        R8_UInt,
        R8_SInt,
        RG8_UNorm,
        RG8_SNorm,
        RG8_UInt,
        RG8_SInt,
        RGBA8_UNorm,
        RGBA8_SNorm,
        RGBA8_UInt,
        RGBA8_SInt,
        R16_UNorm,
        R16_SNorm,
        R16_UInt,
        R16_SInt,
        R16_SFloat,
        RG16_UNorm,
        RG16_SNorm,
        RG16_UInt,
        RG16_SInt,
        RG16_SFloat,
        RGBA16_UNorm,
        RGBA16_SNorm,
        RGBA16_UInt,
        RGBA16_SInt,
        RGBA16_SFloat,
        R32_UInt,
        R32_SInt,
        R32_Float,
        RG32_UInt,
        RG32_SInt,
        RG32_SFloat,
        RGBA32_UInt,
        RGBA32_SInt,
        RGBA32_SFloat,
    };

    [[nodiscard]] inline uint8 TextureFormat_BytesPerPixel(TextureFormat textureFormat) noexcept;

    CHT_STRUCT()
    struct TextureDesc final
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
        uint8              MipmapCount;
        CHT_PROPERTY()
        std::vector<uint8> Data;

        void GenerateMipmaps(bool value);
    };


    // ========================================================================
    // ============================ RenderTexture =============================
    // ========================================================================

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
        // Stencil,
        // DepthStencil,
    };

    // NOTE(v.matushkin): Not sure about this enum and the naming
    enum class RenderTextureUsage : uint8
    {
        Default,
        ShaderRead,
    };

    struct ClearColorValue final
    {
        float32 Value[4];

        [[nodiscard]] static ClearColorValue Default() noexcept { return ClearColorValue{ .Value = {0.f, 0.f, 0.f, 0.f} }; }
    };

    struct ClearDepthStencilValue final
    {
        float32 Depth;
        uint8   Stencil;

        // NOTE(v.matushkin): <ReverseZ> 
        [[nodiscard]] static ClearDepthStencilValue Default() noexcept { return ClearDepthStencilValue{ .Depth = 0.f, .Stencil = 0 }; }
    };

    union RenderTextureClearValue
    {
        ClearColorValue        Color;
        ClearDepthStencilValue DepthStencil;

        [[nodiscard]] static RenderTextureClearValue DefaultColor() noexcept { return RenderTextureClearValue{ .Color = ClearColorValue::Default() }; }
        [[nodiscard]] static RenderTextureClearValue DefaultDepthStencil() noexcept { return RenderTextureClearValue{ .DepthStencil = ClearDepthStencilValue::Default() }; }
    };

    struct RenderTextureDesc final
    {
        std::string             Name;
        RenderTextureClearValue ClearValue;
        uint32                  Width;
        uint32                  Height;
        RenderTextureFormat     Format;
        RenderTextureUsage      Usage;

        [[nodiscard]] RenderTextureType RenderTextureType() const noexcept;
    };


    // ========================================================================
    // ============================== RenderPass ==============================
    // ========================================================================

    enum class AttachmentLoadAction : uint8
    {
        Clear,
        DontCare,
    };

    struct AttachmentDesc final
    {
        RenderTextureHandle  RTHandle;
        AttachmentLoadAction LoadAction;
    };

    struct SubpassDesc final
    {
        std::vector<uint8> ColorAttachmentIndices;
        bool               UseDepthStencilAttachment; // NOTE(v.matushkin): Questionable?
    };

    struct RenderPassDesc final
    {
        std::vector<AttachmentDesc>   ColorAttachments;
        std::optional<AttachmentDesc> DepthStencilAttachment;
        SubpassDesc                   Subpass; // NOTE(v.matushkin): Only one for now
    };


    // ========================================================================
    // =============================== Sampler ================================
    // ========================================================================

    // NOTE(v.matushkin): Vulkan has "Cubic", thats why I made separate SamplerMipmapMode
    enum class SamplerFilterMode : uint8
    {
        Nearest,
        Linear,
    };

    enum class SamplerMipmapMode : uint8
    {
        Nearest,
        Linear,
    };

    enum class SamplerAddressMode : uint8
    {
        ClampToEdge,
        ClampToBorder,
        MirroredOnce,
        MirroredRepeat,
        Repeat,
    };

    struct SamplerDesc final
    {
        SamplerFilterMode  MinFilter;
        SamplerFilterMode  MagFilter;
        SamplerMipmapMode  MipmapFilter;
        uint8              AnisoLevel;
        SamplerAddressMode AddressModeU;
        SamplerAddressMode AddressModeV;
        SamplerAddressMode AddressModeW;
        float32            MipLodBias;

        void SetNearestFilter() noexcept;   // Min/Mag Nearest, Mip Nearest
        void SetBilinearFilter() noexcept;  // Min/Mag Linear,  Mip Nearest
        void SetTrilinearFilter() noexcept; // Min/Mag Linear,  Mip Linear
        void SetAnisotropicFilter(uint8 anisoLevel);
        void SetAddressMode(SamplerAddressMode uvw) noexcept { AddressModeU = uvw; AddressModeV = uvw; AddressModeW = uvw; }

        [[nodiscard]] static SamplerDesc Default() noexcept;
    };


    // ========================================================================
    // ============================== Swapchain ===============================
    // ========================================================================

    struct SwapchainDesc final
    {
        uint64              WindowNativeHandle;
        uint16              Width;
        uint16              Height;
        uint8               BufferCount;
        RenderTextureFormat Format;
    };


    // ========================================================================
    // ================================ Shader ================================
    // ========================================================================

    //- RasterizerState

    CHT_ENUM()
    enum class PolygonMode : uint8
    {
        Fill,
        Wireframe,
    };

    CHT_ENUM()
    enum class CullMode : uint8
    {
        Off,
        Front,
        Back,
        // OpenGl and Vulkan have FrontAndBack
    };

    CHT_ENUM()
    enum class TriangleFrontFace : uint8
    {
        Clockwise,
        CounterClockwise,
    };

    struct RasterizerStateDesc final
    {
        PolygonMode       PolygonMode;
        CullMode          CullMode;
        TriangleFrontFace FrontFace;

        [[nodiscard]] static RasterizerStateDesc Default() noexcept;
    };

    //- DepthStencilState

    // TODO(v.matushkin): What the fuck is this? I don't remember making it.
    //  It's just true/false for DepthStencilState.
    //  Looks like this was made to set depth RenderTexture access in RenderPasses may be?
    enum class DepthTest : uint8
    {
        Off,
        ReadOnly,
        ReadWrite,
    };

    CHT_ENUM()
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

    struct DepthStencilStateDesc final
    {
        bool            DepthTestEnable;
        bool            DepthWriteEnable;
        CompareFunction DepthCompareFunction;

        [[nodiscard]] static DepthStencilStateDesc Default() noexcept;
    };

    //- BlendState

    CHT_ENUM()
    enum class BlendMode : uint8
    {
        Off,
        BlendOp,
        LogicOp,
    };

    CHT_ENUM()
    enum class BlendOp : uint8
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max,
    };

    CHT_ENUM()
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

    CHT_ENUM()
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

    struct BlendStateDesc final
    {
        BlendMode    BlendMode;
        BlendFactor  ColorSrcBlendFactor;
        BlendFactor  ColorDstBlendFactor;
        BlendOp      ColorBlendOp;
        BlendFactor  AlphaSrcBlendFactor;
        BlendFactor  AlphaDstBlendFactor;
        BlendOp      AlphaBlendOp;
        BlendLogicOp LogicOp;

        [[nodiscard]] static BlendStateDesc Default() noexcept;
    };

    struct ShaderBlob final
    {
        uint64                   Size;
        std::unique_ptr<uint8[]> Data;
    };

    CHT_STRUCT()
    struct ShaderDesc final
    {
        CHT_GENERATED_BODY()

        CHT_PROPERTY()
        std::string           Name;
        RasterizerStateDesc   RasterizerStateDesc;
        DepthStencilStateDesc DepthStencilStateDesc;
        BlendStateDesc        BlendStateDesc;
        ShaderBlob            VertexBlob;
        ShaderBlob            FragmentBlob;
    };


    GraphicsBufferDesc GraphicsBufferDesc::Vertex(uint32 elementCount, uint32 elementSize) noexcept
    {
        return GraphicsBufferDesc(elementCount, elementSize, GraphicsBufferUsage::Vertex);
    }

    GraphicsBufferDesc GraphicsBufferDesc::Index(uint32 elementCount, uint32 elementSize) noexcept
    {
        return GraphicsBufferDesc(elementCount, elementSize, GraphicsBufferUsage::Index);
    }

    GraphicsBufferDesc GraphicsBufferDesc::Constant(uint32 elementCount, uint32 elementSize) noexcept
    {
        return GraphicsBufferDesc(elementCount, elementSize, GraphicsBufferUsage::Constant);
    }


    void SamplerDesc::SetNearestFilter() noexcept
    {
        MinFilter = MagFilter = SamplerFilterMode::Nearest;
        MipmapFilter = SamplerMipmapMode::Nearest;
        AnisoLevel = 0;
    }

    void SamplerDesc::SetBilinearFilter() noexcept
    {
        MinFilter = MagFilter = SamplerFilterMode::Linear;
        MipmapFilter = SamplerMipmapMode::Nearest;
        AnisoLevel = 0;
    }

    void SamplerDesc::SetTrilinearFilter() noexcept
    {
        MinFilter = MagFilter = SamplerFilterMode::Linear;
        MipmapFilter = SamplerMipmapMode::Linear;
        AnisoLevel = 0;
    }

    SamplerDesc SamplerDesc::Default() noexcept
    {
        return SamplerDesc{
            .MinFilter    = SamplerFilterMode::Linear,
            .MagFilter    = SamplerFilterMode::Linear,
            .MipmapFilter = SamplerMipmapMode::Linear,
            .AnisoLevel   = 16,
            .AddressModeU = SamplerAddressMode::Repeat,
            .AddressModeV = SamplerAddressMode::Repeat,
            .AddressModeW = SamplerAddressMode::Repeat,
            .MipLodBias   = 0.0f,
        };
    }


    RasterizerStateDesc RasterizerStateDesc::Default() noexcept
    {
        return RasterizerStateDesc{
            .PolygonMode = PolygonMode::Fill,
            .CullMode    = CullMode::Back,
            .FrontFace   = TriangleFrontFace::CounterClockwise,
        };
    }

    DepthStencilStateDesc DepthStencilStateDesc::Default() noexcept
    {
        return DepthStencilStateDesc{
            .DepthTestEnable      = true,
            .DepthWriteEnable     = true,
            .DepthCompareFunction = CompareFunction::GreaterEqual, // NOTE(v.matushkin: <ReverseZ>
        };
    }

    BlendStateDesc BlendStateDesc::Default() noexcept
    {
        return BlendStateDesc{
            .BlendMode           = BlendMode::Off,
            .ColorSrcBlendFactor = BlendFactor::One,
            .ColorDstBlendFactor = BlendFactor::Zero,
            .ColorBlendOp        = BlendOp::Add,
            .AlphaSrcBlendFactor = BlendFactor::One,
            .AlphaDstBlendFactor = BlendFactor::Zero,
            .AlphaBlendOp        = BlendOp::Add,
            .LogicOp             = BlendLogicOp::Noop,
        };
    }

} // export namespace Copium
