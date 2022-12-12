export module CopiumEngine.Assets.Model;

import Copium.Core;
import Copium.Math;

import CopiumEngine.Assets.Material;
import CopiumEngine.Assets.Mesh;

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

        Float3                                  Position;
        float32                                 Scale;
        Quaternion                              Rotation;

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
