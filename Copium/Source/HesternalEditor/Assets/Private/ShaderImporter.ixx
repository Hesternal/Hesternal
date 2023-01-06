export module HesternalEditor.Assets.ShaderImporter;

import HesternalEngine.Assets.Shader;

import <string>;


export namespace Hesternal
{

    class ShaderImporter final
    {
    public:
        [[nodiscard]] static Shader Import(const std::string& shaderPath);
    };

} // export namespace Hesternal
