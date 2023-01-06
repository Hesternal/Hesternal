export module CopiumEditor.Assets.ShaderImporter;

import CopiumEngine.Assets.Shader;

import <string>;


export namespace Copium
{

    class ShaderImporter final
    {
    public:
        [[nodiscard]] static Shader Import(const std::string& shaderPath);
    };

} // export namespace Copium
