export module HesternalEngine.Graphics.RenderData;

import Hesternal.Core;
import Hesternal.Math;

import HesternalEngine.Assets.Material;
import HesternalEngine.Assets.Mesh;

import <memory>;
import <vector>;


export namespace Hesternal
{

    // NOTE(v.matushkin): I don't know why I have this structures that just duplicate Camera and DirectionalLight components

    struct CameraRenderData final
    {
        Float4x4 View;
        Float4x4 Projection;
        Float4x4 ViewProjection;
        Float3   Position;
    };

    struct DirectionalLightRenderData final
    {
        Float3 Forward;
        Float3 Color;
    };

    struct PointLightRenderData final
    {
        Float3  Position;
        Float3  Color;
        float32 DistanceAttenuationScale;
    };

    struct SpotLightRenderData final
    {
        Float3  Position;
        Float3  Forward;
        Float3  Color;
        float32 DistanceAttenuationScale;
        float32 AngleAttenuationScale;
        float32 AngleAttenuationOffset;
    };

    struct EntityRenderData final
    {
        Float4x4 LocalToWorld;
        uint32   MaterialIndex;
        uint32   MeshIndex;
    };

    struct RenderData final
    {
        CameraRenderData                        Camera;
        std::vector<DirectionalLightRenderData> DirectionalLights;
        std::vector<PointLightRenderData>       PointLights;
        std::vector<SpotLightRenderData>        SpotLights;
        std::vector<std::shared_ptr<Material>>  Materials;
        std::vector<std::shared_ptr<Mesh>>      Meshes;
        std::vector<EntityRenderData>           Entities;
    };

} // export namespace Hesternal
