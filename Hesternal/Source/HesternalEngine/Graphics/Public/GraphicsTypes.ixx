module;

#include "Hesternal/Core/Defines.hpp"
#include "EngineCht/GraphicsTypes.cht.hpp"

export module HesternalEngine.Graphics.GraphicsTypes;

import Hesternal.Core;

import HesternalEngine.Core.ForwardDeclaration;

import <memory>;
import <optional>;
import <string>;
import <vector>;


namespace
{
    using namespace Hesternal;


    inline constexpr uint32 k_InvalidHandle = UInt32::Max;

} // namespace


export namespace Hesternal
{

    enum class GraphicsBufferHandle : uint32 { Invalid = k_InvalidHandle };
    enum class RenderPassHandle     : uint32 { Invalid = k_InvalidHandle };
    enum class RenderTextureHandle  : uint32 { Invalid = k_InvalidHandle };
    enum class ShaderHandle         : uint32 { Invalid = k_InvalidHandle };
    enum class SwapchainHandle      : uint32 { Invalid = k_InvalidHandle };
    enum class TextureHandle        : uint32 { Invalid = k_InvalidHandle };


    // Can't forward declare for GraphicsBufferDesc::Index because of fucking HeaderTool
    enum class IndexFormat : uint8
    {
        UInt16,
        UInt32,
    };


    // ========================================================================
    // ============================ GraphicsBuffer ============================
    // ========================================================================

    enum class GraphicsBufferUsage : uint8
    {
        Vertex,
        Index,
        Constant,
        Structured,
        Invalid = UInt8::Max,
    };

    struct GraphicsBufferDesc final
    {
        uint32              ElementCount;
        uint32              ElementSize;
        GraphicsBufferUsage Usage;

        [[nodiscard]] uint32 SizeInBytes() const noexcept { return ElementCount * ElementSize; }

        [[nodiscard]] static constexpr GraphicsBufferDesc Vertex(uint32 elementCount, uint32 elementSize) noexcept;
        [[nodiscard]] static constexpr GraphicsBufferDesc Index(uint32 elementCount, IndexFormat indexFormat) noexcept;
        [[nodiscard]] static constexpr GraphicsBufferDesc Constant(uint32 elementCount, uint32 elementSize) noexcept;
        [[nodiscard]] static constexpr GraphicsBufferDesc Structured(uint32 elementCount, uint32 elementSize) noexcept;
        [[nodiscard]] static constexpr GraphicsBufferDesc Invalid() noexcept;
    };


    // ========================================================================
    // ================================= Mesh =================================
    // ========================================================================

    //enum class IndexFormat : uint8
    //{
    //    UInt16,
    //    UInt32,
    //};

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

    struct VertexAttributeDesc final
    {
        uint8                 Offset;
        uint8                 Dimension;
        uint8                 Stream;
        VertexAttribute       Attribute; // NOTE(v.matushkin): Do I need this?
        VertexAttributeFormat Format;

        [[nodiscard]] uint32 Stride() const noexcept;

        [[nodiscard]] static constexpr VertexAttributeDesc Position() noexcept;
        [[nodiscard]] static constexpr VertexAttributeDesc Normal() noexcept;
        [[nodiscard]] static constexpr VertexAttributeDesc UV0() noexcept;
    };

    struct SubMeshDesc final
    {
        IndexFormat IndexFormat;
        uint32      IndexCount;
        uint32      IndexBufferOffset;
        uint32      VertexCount;
        uint32      PositionBufferOffset;
        uint32      NormalBufferOffset;
        uint32      UV0BufferOffset;
    };

    struct MeshDesc final
    {
        std::string              Name;
        VertexAttributeDesc      Position;
        VertexAttributeDesc      Normal;
        VertexAttributeDesc      UV0;
        std::vector<SubMeshDesc> SubMeshes;
        // NOTE(v.matushkin): *Data should be of some Array type, so there will be no memory wasted,
        //   and no way to change the size of this containers.
        std::vector<uint8>       IndexData;
        std::vector<uint8>       VertexData;
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


    constexpr GraphicsBufferDesc GraphicsBufferDesc::Vertex(uint32 elementCount, uint32 elementSize) noexcept
    {
        return GraphicsBufferDesc{ .ElementCount = elementCount, .ElementSize = elementSize, .Usage = GraphicsBufferUsage::Vertex };
    }

    constexpr GraphicsBufferDesc GraphicsBufferDesc::Index(uint32 elementCount, IndexFormat indexFormat) noexcept
    {
        return GraphicsBufferDesc{
            .ElementCount = elementCount,
            .ElementSize  = indexFormat == IndexFormat::UInt16 ? uint32(2) : uint32(4),
            .Usage        = GraphicsBufferUsage::Index
        };
    }

    constexpr GraphicsBufferDesc GraphicsBufferDesc::Constant(uint32 elementCount, uint32 elementSize) noexcept
    {
        return GraphicsBufferDesc{ .ElementCount = elementCount, .ElementSize = elementSize, .Usage = GraphicsBufferUsage::Constant };
    }

    constexpr GraphicsBufferDesc GraphicsBufferDesc::Structured(uint32 elementCount, uint32 elementSize) noexcept
    {
        return GraphicsBufferDesc{ .ElementCount = elementCount, .ElementSize = elementSize, .Usage = GraphicsBufferUsage::Structured };
    }

    constexpr GraphicsBufferDesc GraphicsBufferDesc::Invalid() noexcept
    {
        return GraphicsBufferDesc{ .ElementCount = 0, .ElementSize = 0, .Usage = GraphicsBufferUsage::Invalid };
    }


    constexpr VertexAttributeDesc VertexAttributeDesc::Position() noexcept
    {
        return VertexAttributeDesc{
            .Offset    = 0,
            .Dimension = 3,
            .Stream    = 0,
            .Attribute = VertexAttribute::Position,
            .Format    = VertexAttributeFormat::Float32,
        };
    }

    constexpr VertexAttributeDesc VertexAttributeDesc::Normal() noexcept
    {
        return VertexAttributeDesc{
            .Offset    = 0,
            .Dimension = 3,
            .Stream    = 1,
            .Attribute = VertexAttribute::Normal,
            .Format    = VertexAttributeFormat::Float32,
        };
    }

    constexpr VertexAttributeDesc VertexAttributeDesc::UV0() noexcept
    {
        return VertexAttributeDesc{
            .Offset    = 0,
            .Dimension = 2,
            .Stream    = 2,
            .Attribute = VertexAttribute::UV0,
            .Format    = VertexAttributeFormat::Float32,
        };
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
            .DepthCompareFunction = CompareFunction::GreaterEqual, // NOTE(v.matushkin): <ReverseZ>
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

} // export namespace Hesternal
