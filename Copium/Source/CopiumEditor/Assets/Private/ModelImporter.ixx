export module CopiumEditor.Assets.ModelImporter;

import CopiumEngine.Assets.Model;

import <string>;


export namespace Copium
{

    class ModelImporter final
    {
    public:
        [[nodiscard]] static ModelScene Import(const std::string& modelPath);
    };

} // export namespace Copium
