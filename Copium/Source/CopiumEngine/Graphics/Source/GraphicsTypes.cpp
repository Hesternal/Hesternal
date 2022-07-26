module CopiumEngine.Graphics.GraphicsTypes;

import CopiumEngine.Core.CoreTypes;

import <utility>;


namespace Copium
{

    uint32 VertexAttributeDesc::Stride() const
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

    VertexAttributeDesc VertexAttributeDesc::Position()
    {
        return VertexAttributeDesc{
            .Offset    = 0,
            .Dimension = 3,
            .Stream    = 0,
            .Attribute = VertexAttribute::Position,
            .Format    = VertexAttributeFormat::Float32,
        };
    }

    VertexAttributeDesc VertexAttributeDesc::Normal()
    {
        return VertexAttributeDesc{
            .Offset    = 0,
            .Dimension = 3,
            .Stream    = 1,
            .Attribute = VertexAttribute::Normal,
            .Format    = VertexAttributeFormat::Float32,
        };
    }

    VertexAttributeDesc VertexAttributeDesc::UV0()
    {
        return VertexAttributeDesc{
            .Offset    = 0,
            .Dimension = 2,
            .Stream    = 2,
            .Attribute = VertexAttribute::UV0,
            .Format    = VertexAttributeFormat::Float32,
        };
    }


    ClearColorValue ClearColorValue::Default()
    {
        return ClearColorValue{ .Value = {0.f, 0.f, 0.f, 0.f} };
    }

    ClearDepthStencilValue ClearDepthStencilValue::Default()
    {
        return ClearDepthStencilValue{ .Depth = 1.f, .Stencil = 0 };
    }

    RenderTextureClearValue RenderTextureClearValue::DefaultColor()
    {
        return RenderTextureClearValue{ .Color = ClearColorValue::Default() };
    }

    RenderTextureClearValue RenderTextureClearValue::DefaultDepthStencil()
    {
        return RenderTextureClearValue{ .DepthStencil = ClearDepthStencilValue::Default() };
    }


    RenderTextureType RenderTextureDesc::RenderTextureType() const
    {
        if (Format == RenderTextureFormat::BGRA8)
        {
            return RenderTextureType::Color;
        }

        return RenderTextureType::Depth;
    }


    RasterizerStateDesc RasterizerStateDesc::Default()
    {
        return RasterizerStateDesc{
            .PolygonMode = PolygonMode::Fill,
            .CullMode    = CullMode::Back,
            .FrontFace   = TriangleFrontFace::CounterClockwise,
        };
    }

    DepthStencilStateDesc DepthStencilStateDesc::Default()
    {
        return DepthStencilStateDesc{
            .DepthTestEnable      = true,
            .DepthWriteEnable     = true,
            .DepthCompareFunction = CompareFunction::Less,
        };
    }

    BlendStateDesc BlendStateDesc::Default()
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

} // namespace Copium
