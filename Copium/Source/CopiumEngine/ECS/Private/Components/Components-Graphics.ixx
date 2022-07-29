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
        Float4x4 Projection;
    };

    struct RenderMesh final : IComponent
    {
        std::shared_ptr<Mesh>     Mesh;
        std::shared_ptr<Material> Material;
    };

} // export namespace Copium
