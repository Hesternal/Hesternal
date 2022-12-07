export module CopiumEngine.ECS.Components:Graphics;

import :IComponent;
import CopiumEngine.Assets.Material;
import CopiumEngine.Assets.Mesh;
import CopiumEngine.Math;

import <memory>;


export namespace Copium
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

} // export namespace Copium
