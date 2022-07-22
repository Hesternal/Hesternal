module;

#include <string>

export module CopiumEditor.Assets.ModelImporter;

import CopiumEngine.Assets.NativeAsset;


export namespace Copium
{

    class ModelImporter final
    {
    public:
        [[nodiscard]] static ModelAsset Import(const std::string& meshPath);
    };

} // export namespace Copium
