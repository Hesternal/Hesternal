module;

#include "Engine/Core/Debug.hpp"

module CopiumEditor.Assets.ShaderImporter;

import CopiumEngine.Assets.Shader;
import CopiumEngine.Core.Platform.File;

import CopiumEditor.Assets.DX11ShaderCompiler;
import CopiumEditor.Assets.ParsedShaderDesc;
import CopiumEditor.Assets.ShaderImporter.ParserError;
import CopiumEditor.Assets.ShaderImporter.ShaderParser;

import <string>;
import <utility>;


namespace Copium
{

    Shader ShaderImporter::Import(const std::string& shaderPath)
    {
        const std::string shaderFileContent = File::ReadAllText(shaderPath);

        try
        {
            ParsedShaderDesc parsedShaderDesc = ShaderParser::Parse(shaderFileContent);
            ShaderDesc shaderDesc = {
                .RasterizerStateDesc   = parsedShaderDesc.RasterizerStateDesc,
                .DepthStencilStateDesc = parsedShaderDesc.DepthStencilStateDesc,
                .BlendStateDesc        = parsedShaderDesc.BlendStateDesc,
            };

            DX11ShaderCompiler dx11ShaderCompiler;
            dx11ShaderCompiler.Compile(parsedShaderDesc, shaderDesc);

            shaderDesc.Name = std::move(parsedShaderDesc.Name);

            return Shader(std::move(shaderDesc));
        }
        catch ([[maybe_unused]] const ParserError& e)
        {
            COP_LOG_CRITICAL(e.Message);
            throw;
        }
    }

} // namespace Copium
