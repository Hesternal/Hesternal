export module CopiumEditor.Assets.ModelImporter;

import CopiumEngine.ECS.Entity;

import <string>;


export namespace Copium
{

    class ModelImporter final
    {
    public:
        [[nodiscard]] static Entity Import(const std::string& modelPath);
    };

} // export namespace Copium
