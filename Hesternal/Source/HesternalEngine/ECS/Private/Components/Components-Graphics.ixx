export module HesternalEngine.ECS.Components:Graphics;

import Hesternal.ECS;
import Hesternal.Math;

import HesternalEngine.Assets.Material;
import HesternalEngine.Assets.Mesh;

import <memory>;


// TODO(v.matushkin): Intensity should be expressed in some physical units like Lumen/Candela/Lux,
//   not just some random float. But right now I don't wanna do the math for it.


export namespace Hesternal
{

    struct Camera final : IComponent
    {
        Float4x4 View;
        Float4x4 Projection;
        Float4x4 ViewProjection;
    };

    // NOTE(v.matushkin): I think at least non directional lights can be combined in one component?
    //   But for now lets keep it simple.

    struct DirectionalLight final : IComponent
    {
        Color Color;
    };

    struct PointLight final : IComponent
    {
        Color   Color;
        float32 Intensity;
        float32 Range;
    };

    // NOTE(v.matushkin): How to store angle: degrees/rads/cos ?
    //   Since it probably wouldn't change often(or at all), it would be better to directly store cos value of the angle.
    //   But right now it will be in degrees and conversion will happen in GraphicsSystem.
    struct SpotLight final : IComponent
    {
        Color   Color;
        float32 Intensity;
        float32 Range;
        // NOTE(v.matushkin): May be better names would be OuterAngle/InnerAnglePercent ?
        float32 SpotAngle; /// in degrees
        float32 InnerSpotPercent;
    };

    struct RenderMesh final : IComponent
    {
        std::shared_ptr<Mesh>     Mesh;
        std::shared_ptr<Material> Material;
    };

} // export namespace Hesternal
