export module HesternalEditor.Assets.ModelImporter;

import HesternalEngine.Assets.Model;

import <string>;


export namespace Hesternal
{

    class ModelImporter final
    {
    public:
        [[nodiscard]] static ModelScene Import(const std::string& modelPath);
    };

} // export namespace Hesternal
