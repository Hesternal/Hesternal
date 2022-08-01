export module CopiumEditor.Assets.ShaderImporter.ShaderParser;

import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Utils.EnumUtils;

import CopiumEditor.Assets.ShaderImporter.BaseParser;

import <string>;


export namespace Copium
{

    class ShaderParser final : public BaseParser
    {
    public:
        [[nodiscard]] static ShaderDesc Parse(std::string_view shaderCode);

    private:
        ShaderParser(std::string_view shaderCode);
        ~ShaderParser() = default;

        ShaderParser(ShaderParser&& other) noexcept = default;
        ShaderParser& operator=(ShaderParser&& other) noexcept = default;

        [[nodiscard]] ShaderDesc _Parse();

        [[nodiscard]] std::string _ParseShaderName();
        [[nodiscard]] std::string _ParseShaderProgram();

        [[nodiscard]] void            _ParseShaderState(ShaderDesc& shaderDesc);
        [[nodiscard]] bool            _ParseBoolValue();

        // NOTE(v.matushkin): CChtEnum constraint is not enough, since I call this method with only some of the GraphicsTypes enums
        // NOTE(v.matushkin): Importing EnumUtils just for the CChtEnum concept?
        template<EnumUtils::CChtEnum TEnum>
        [[nodiscard]] TEnum           _ParseEnumValue();

        [[nodiscard]] void            _ParseBlendOpValue(BlendStateDesc& blendStateDesc);
        [[nodiscard]] void            _ParseBlendFactorValue(BlendStateDesc& blendStateDesc);
    };

} // export namespace Copium
