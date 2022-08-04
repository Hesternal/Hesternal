module;

#include "Engine/Core/Debug.hpp"

module CopiumEditor.Assets.ShaderImporter.ShaderParser;

import CopiumEngine.Core.CoreTypes;


// TODO(v.matushkin): Check for invalid State parsed values. EnumUtils doesn't check if it's a valid value.


namespace ShaderIdentifier
{

    static constexpr std::string_view Shader   = "Shader";
    static constexpr std::string_view State    = "State";
    static constexpr std::string_view Vertex   = "Vertex";
    static constexpr std::string_view Fragment = "Fragment";

} // namespace ShaderIdentifier

namespace StateIdentifier
{

    //- RasterizerState
    static constexpr std::string_view Polygon      = "Polygon";
    static constexpr std::string_view Cull         = "Cull";
    //- DepthStencilState
    static constexpr std::string_view DepthTest    = "DepthTest";
    static constexpr std::string_view DepthWrite   = "DepthWrite";
    static constexpr std::string_view DepthCompare = "DepthCompare";
    //- BlendState
    static constexpr std::string_view BlendOp      = "BlendOp";
    static constexpr std::string_view Blend        = "Blend";

} // namespace StateIdentifier

namespace StateValueIdentifier
{

    static constexpr std::string_view On  = "On";
    static constexpr std::string_view Off = "Off";

} // namespace StateValueIdentifier


namespace Copium
{

    ParsedShaderDesc ShaderParser::Parse(std::string_view shaderCode)
    {
        return ShaderParser(shaderCode)._Parse();
    }


    ShaderParser::ShaderParser(std::string_view shaderCode)
        : BaseParser(shaderCode)
    {
    }


    ParsedShaderDesc ShaderParser::_Parse()
    {
        ParsedShaderDesc parsedShaderDesc = {
            .RasterizerStateDesc   = RasterizerStateDesc::Default(),
            .DepthStencilStateDesc = DepthStencilStateDesc::Default(),
            .BlendStateDesc        = BlendStateDesc::Default(),
        };

        parsedShaderDesc.Name = _ParseShaderName();

        ExpectSymbol('{');

        bool foundShaderState = false;
        bool foundVertexProgram = false;
        bool foundFragmentProgram = false;

        while (true)
        {
            const Token token = NextToken();

            if (token.IsEndOfFile())
            {
                throw Error_UnexpectedEndOfFile(token);
            }

            if (token.IsSymbol())
            {
                if (token.IsValue('}'))
                {
                    break;
                }
            }
            else if (token.IsIdentifier())
            {
                if (token.IsValue(ShaderIdentifier::State))
                {
                    if (foundShaderState)
                    {
                        throw Error_Message("Found more than one State block", token);
                    }
                    foundShaderState = true;
                    _ParseShaderState(parsedShaderDesc);
                }
                else if (token.IsValue(ShaderIdentifier::Vertex))
                {
                    if (foundVertexProgram)
                    {
                        throw Error_Message("Found more than one Vertex program", token);
                    }
                    foundVertexProgram = true;
                    parsedShaderDesc.VertexSource = _ParseShaderProgram();
                }
                else if (token.IsValue(ShaderIdentifier::Fragment))
                {
                    if (foundFragmentProgram)
                    {
                        throw Error_Message("Found more than one Fragment program", token);
                    }
                    foundFragmentProgram = true;
                    parsedShaderDesc.FragmentSource = _ParseShaderProgram();
                }
                else
                {
                    throw Error_UnexpectedIdentifier(token);
                }
            }
            else
            {
                throw Error_UnexpectedToken(token);
            }
        }

        if (foundVertexProgram == false)
        {
            throw ParserError("Vertex program was not defined");
        }
        if (foundFragmentProgram == false)
        {
            throw ParserError("Fragment program was not defined");
        }

        return parsedShaderDesc;
    }

    std::string ShaderParser::_ParseShaderName()
    {
        ExpectIdentifier(ShaderIdentifier::Shader);
        return std::string(ExpectString().Value);
    }


    void ShaderParser::_ParseShaderState(ParsedShaderDesc& parsedShaderDesc)
    {
        ExpectSymbol('{');

        bool foundPolygon = false;
        bool foundCull = false;
        bool foundDepthTest = false;
        bool foundDepthWrite = false;
        bool foundDepthCompare = false;
        bool foundBlendOp = false;
        bool foundBlend = false;

        while (PeekToken().IsSymbol('}') == false)
        {
            const Token token = ExpectIdentifier();

            //- RasterizerState
            if (token.IsValue(StateIdentifier::Polygon))
            {
                if (foundPolygon)
                {
                    throw Error_Message("Found more than one Polygon value", token);
                }
                foundPolygon = true;
                parsedShaderDesc.RasterizerStateDesc.PolygonMode = _ParseEnumValue<PolygonMode>();
            }
            else if (token.IsValue(StateIdentifier::Cull))
            {
                if (foundCull)
                {
                    throw Error_Message("Found more than one Cull value", token);
                }
                foundCull = true;
                parsedShaderDesc.RasterizerStateDesc.CullMode = _ParseEnumValue<CullMode>();
            }
            //- DepthStencilState
            else if (token.IsValue(StateIdentifier::DepthTest))
            {
                if (foundDepthTest)
                {
                    throw Error_Message("Found more than one DepthTest value", token);
                }
                foundDepthTest = true;
                parsedShaderDesc.DepthStencilStateDesc.DepthTestEnable = _ParseBoolValue();
            }
            else if (token.IsValue(StateIdentifier::DepthWrite))
            {
                if (foundDepthWrite)
                {
                    throw Error_Message("Found more than one DepthWrite value", token);
                }
                foundDepthWrite = true;
                parsedShaderDesc.DepthStencilStateDesc.DepthWriteEnable = _ParseBoolValue();
            }
            else if (token.IsValue(StateIdentifier::DepthCompare))
            {
                if (foundDepthCompare)
                {
                    throw Error_Message("Found more than one DepthCompare value", token);
                }
                foundDepthCompare = true;
                parsedShaderDesc.DepthStencilStateDesc.DepthCompareFunction = _ParseEnumValue<CompareFunction>();
            }
            //- BlendState
            else if (token.IsValue(StateIdentifier::BlendOp))
            {
                if (foundBlendOp)
                {
                    throw Error_Message("Found more than one BlendOp value", token);
                }
                foundBlendOp = true;
                _ParseBlendOpValue(parsedShaderDesc.BlendStateDesc);
            }
            else if (token.IsValue(StateIdentifier::Blend))
            {
                if (foundBlend)
                {
                    throw Error_Message("Found more than one Blend value", token);
                }
                foundBlend = true;
                _ParseBlendFactorValue(parsedShaderDesc.BlendStateDesc);
            }
            else
            {
                throw Error_UnexpectedToken(token);
            }
        }

        // Consume '}' symbol
        AdvanceToken();

        if (foundBlendOp || foundBlend)
        {
            parsedShaderDesc.BlendStateDesc.BlendMode = BlendMode::BlendOp;
        }
    }

    bool ShaderParser::_ParseBoolValue()
    {
        const Token token = ExpectIdentifier();

        if (token.IsValue(StateValueIdentifier::On))
        {
            return true;
        }
        else if (token.IsValue(StateValueIdentifier::Off))
        {
            return false;
        }

        throw Error_UnexpectedIdentifier(token);
    }

    template<EnumUtils::CChtEnum TEnum>
    TEnum ShaderParser::_ParseEnumValue()
    {
        return EnumUtils::FromString<TEnum>(ExpectIdentifier().Value);
    }

    // BlendOp ColorBlendOp [, AlphaBlendOp]
    void ShaderParser::_ParseBlendOpValue(BlendStateDesc& blendStateDesc)
    {
        blendStateDesc.ColorBlendOp = _ParseEnumValue<BlendOp>();

        if (PeekToken().IsSymbol(','))
        {
            AdvanceToken();
            blendStateDesc.AlphaBlendOp = _ParseEnumValue<BlendOp>();
        }
        else
        {
            blendStateDesc.AlphaBlendOp = blendStateDesc.ColorBlendOp;
        }
    }

    // Blend ColorSrcBlendFactor ColorDstBlendFactor [, AlphaSrcBlendFactor AlphaDstBlendFactor]
    void ShaderParser::_ParseBlendFactorValue(BlendStateDesc& blendStateDesc)
    {
        blendStateDesc.ColorSrcBlendFactor = _ParseEnumValue<BlendFactor>();
        blendStateDesc.ColorDstBlendFactor = _ParseEnumValue<BlendFactor>();

        if (PeekToken().IsSymbol(','))
        {
            AdvanceToken();
            blendStateDesc.AlphaSrcBlendFactor = _ParseEnumValue<BlendFactor>();
            blendStateDesc.AlphaDstBlendFactor = _ParseEnumValue<BlendFactor>();
        }
        else
        {
            blendStateDesc.AlphaSrcBlendFactor = blendStateDesc.ColorSrcBlendFactor;
            blendStateDesc.AlphaDstBlendFactor = blendStateDesc.ColorDstBlendFactor;
        }
    }


    std::string ShaderParser::_ParseShaderProgram()
    {
        ExpectSymbol('{');

        Token token = PeekToken();

        if (token.IsEndOfFile())
        {
            throw Error_UnexpectedEndOfFile(token);
        }
        else if (token.IsSymbol('}'))
        {
            throw Error_Message("Empty program definition", token);
        }

        const char* const shaderProgramBegin = token.Value.data();

        uint32 openBraces = 1;

        do
        {
            token = NextToken();

            if (token.IsEndOfFile())
            {
                throw Error_UnexpectedEndOfFile(token);
            }

            if (token.IsSymbol())
            {
                if (token.IsValue('{'))
                {
                    openBraces++;
                }
                else if (token.IsValue('}'))
                {
                    openBraces--;
                }
            }
        } while (openBraces);

        return std::string(shaderProgramBegin, token.Value.data());
    }

} // namespace Copium
