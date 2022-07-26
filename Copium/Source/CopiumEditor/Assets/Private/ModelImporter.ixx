module;

#include <string>

export module CopiumEditor.Assets.ModelImporter;

import CopiumEngine.ECS.Entity;


export namespace Copium
{

    class ModelImporter final
    {
    public:
        [[nodiscard]] static Entity Import(const std::string& modelPath);
    };

} // export namespace Copium
