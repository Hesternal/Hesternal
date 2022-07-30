export module CopiumEngine.Assets.Model;

import CopiumEngine.Assets.Material;
import CopiumEngine.Assets.Mesh;
import CopiumEngine.Core.CoreTypes;

import <memory>;
import <string>;
import <vector>;


export namespace Copium
{

    struct ModelMesh final
    {
        std::shared_ptr<Mesh> Mesh;
        // NOTE(v.matushkin): What if I want to remap/override original material to some external?
        uint32                MaterialIndex;
    };

    struct ModelNode final
    {
        std::string                             Name;
        std::vector<std::unique_ptr<ModelNode>> Children;
        // NOTE(v.matushkin): Idk what Assimp means by having multiple meshes per node and how I should handle it
        std::vector<uint32>                     MeshIndices;
    };

    struct ModelScene final
    {
        std::vector<std::shared_ptr<Material>> Materials;
        std::vector<ModelMesh>                 Meshes;
        std::unique_ptr<ModelNode>             RootNode;
    };

} // export namespace Copium
