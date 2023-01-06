export module HesternalEngine.Graphics.RenderData;

import Hesternal.Core;
import Hesternal.Math;

import HesternalEngine.Assets.Material;
import HesternalEngine.Assets.Mesh;

import <memory>;
import <vector>;


export namespace Hesternal
{

    struct CameraRenderData final
    {
        Float4x4 View;
        Float4x4 Projection;
        Float4x4 ViewProjection;
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

} // export namespace Hesternal
