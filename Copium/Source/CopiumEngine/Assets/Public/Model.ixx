export module CopiumEngine.Assets.Model;

import Copium.Core;
import Copium.Math;

import CopiumEngine.Assets.Material;
import CopiumEngine.Assets.Mesh;

import <memory>;
import <optional>;
import <string>;
import <vector>;


export namespace Copium
{

    struct ModelMesh final
    {
        std::shared_ptr<Mesh> Mesh;
        // NOTE(v.matushkin): What if I want to remap/override original material to some external?
        std::vector<uint32>   MaterialIndices;
    };

    struct ModelNode final
    {
        std::string                             Name;
        std::vector<std::unique_ptr<ModelNode>> Children;

        Float3                                  Position;
        float32                                 Scale;
        Quaternion                              Rotation;

        std::optional<uint32>                   MeshIndex;
    };
    // NOTE(v.matushkin): Why with "uint32 MeshIndex" sizeof(ModelNode) == 88,
    //  but with "std::optional<uint32> MeshIndex" it's 96 and not 92 ?
    //static_assert(sizeof(ModelNode) == 96);

    struct ModelScene final
    {
        std::vector<std::shared_ptr<Material>> Materials;
        std::vector<ModelMesh>                 Meshes;
        std::unique_ptr<ModelNode>             RootNode;
    };

} // export namespace Copium
