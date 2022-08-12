export module CopiumEngine.Graphics.RenderData;

import CopiumEngine.Assets.Material;
import CopiumEngine.Assets.Mesh;
import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Math;

import <memory>;
import <vector>;


export namespace Copium
{

    struct CameraRenderData final
    {
        Float4x4 LocalToWorld;
        Float4x4 Projection;
    };

    struct EntityRenderData final
    {
        Float4x4 LocalToWorld;
        uint32   MaterialIndex;
        uint32   MeshIndex;
    };

    struct RenderData final
    {
        CameraRenderData                       Camera;
        std::vector<std::shared_ptr<Material>> Materials;
        std::vector<std::shared_ptr<Mesh>>     Meshes;
        std::vector<EntityRenderData>          Entities;
    };

} // export namespace Copium
