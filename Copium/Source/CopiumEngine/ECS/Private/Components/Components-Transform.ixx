export module CopiumEngine.ECS.Components:Transform;

import :IComponent;

import Copium.Core;
import Copium.Math;

import CopiumEngine.ECS.Entity;

import <vector>;


export namespace Copium
{

    struct Transform final : IComponent
    {
        Float3     Position;
        float32    Scale;
        Quaternion Rotation;

        [[nodiscard]] static inline Transform Identity() noexcept
        {
            return Transform{ .Position = Float3::Zero(), .Scale = 1.0f, .Rotation = Quaternion::Identity() };
        }

        [[nodiscard]] static inline Transform FromScale(float32 scale) noexcept
        {
            return Transform{ .Position = Float3::Zero(), .Scale = scale, .Rotation = Quaternion::Identity() };
        }
    };


    struct LocalToWorld final : IComponent
    {
        Float4x4 Value;
    };

    struct LocalToParent final : IComponent
    {
        Float4x4 Value;
    };


    struct Parent final : IComponent
    {
        Entity Value;
    };

    struct Child final : IComponent
    {
        std::vector<Entity> Children;
    };

} // export namespace Copium
