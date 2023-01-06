export module HesternalEngine.ECS.Components:Graphics;

import Hesternal.ECS;
import Hesternal.Math;

import HesternalEngine.Assets.Material;
import HesternalEngine.Assets.Mesh;

import <memory>;


export namespace Hesternal
{

    struct Camera final : IComponent
    {
        Float4x4 View;
        Float4x4 Projection;
        Float4x4 ViewProjection;
    };

    struct RenderMesh final : IComponent
    {
        std::shared_ptr<Mesh>     Mesh;
        std::shared_ptr<Material> Material;
    };

} // export namespace Hesternal
