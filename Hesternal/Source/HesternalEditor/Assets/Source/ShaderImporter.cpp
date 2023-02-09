module;

#include "Hesternal/Core/Debug.hpp"

module HesternalEditor.Assets.ShaderImporter;

import HesternalEngine.Assets.Shader;
import HesternalEngine.Core.Platform.File;

import HesternalEditor.Assets.DX11ShaderCompiler;
import HesternalEditor.Assets.ParsedShaderDesc;
import HesternalEditor.Assets.ShaderImporter.ParserError;
import HesternalEditor.Assets.ShaderImporter.ShaderParser;

import <string>;
import <utility>;


namespace Hesternal
{

    Shader ShaderImporter::Import(const std::filesystem::path* includeDir, const std::string& shaderPath)
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
            dx11ShaderCompiler.Compile(includeDir, parsedShaderDesc, shaderDesc);

            shaderDesc.Name = std::move(parsedShaderDesc.Name);

            return Shader(std::move(shaderDesc));
        }
        catch ([[maybe_unused]] const ParserError& e)
        {
            HS_LOG_CRITICAL(e.Message);
            throw;
        }
    }

} // namespace Hesternal
