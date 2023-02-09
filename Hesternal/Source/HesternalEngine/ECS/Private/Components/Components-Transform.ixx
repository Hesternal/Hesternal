export module HesternalEngine.ECS.Components:Transform;

import Hesternal.Core;
import Hesternal.ECS;
import Hesternal.Math;

import <vector>;


export namespace Hesternal
{

    struct Transform final : IComponent
    {
        Float3     Position;
        float32    Scale;
        Quaternion Rotation;

        // TODO(v.matushkin): Should I mark these as inline explicitly? Does constexpr still implies inline in modules?
        [[nodiscard]] static inline constexpr Transform Identity() noexcept
        {
            return Transform{ .Position = Float3::Zero(), .Scale = 1.0f, .Rotation = Quaternion::Identity() };
        }

        [[nodiscard]] static inline constexpr Transform FromPosition(Float3 position) noexcept
        {
            return Transform{ .Position = position, .Scale = 1.0f, .Rotation = Quaternion::Identity() };
        }

        [[nodiscard]] static inline constexpr Transform FromScale(float32 scale) noexcept
        {
            return Transform{ .Position = Float3::Zero(), .Scale = scale, .Rotation = Quaternion::Identity() };
        }

        [[nodiscard]] static inline constexpr Transform FromPositionRotationScale(const Float3& position, const Quaternion& rotation, float32 scale) noexcept
        {
            return Transform{ .Position = position, .Scale = scale, .Rotation = rotation };
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

} // export namespace Hesternal
