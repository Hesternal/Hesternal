module;

#include "Engine/Core/Debug.hpp"

module CopiumEditor.Assets.ShaderImporter;

import CopiumEngine.Assets.Shader;
import CopiumEngine.Core.Platform.File;

import CopiumEditor.Assets.ShaderImporter.ParserError;
import CopiumEditor.Assets.ShaderImporter.ShaderParser;

import <string>;


namespace Copium
{

    Shader ShaderImporter::Import(const std::string& shaderPath)
    {
        const std::string shaderFileContent = File::ReadAllText(shaderPath);

        try
        {
            return Shader(ShaderParser::Parse(shaderFileContent));
        }
        catch ([[maybe_unused]] const ParserError& e)
        {
            COP_LOG_CRITICAL(e.Message);
            throw;
        }
    }

} // namespace Copium
