export module CopiumEngine.Math:Transform;

import :Float3;
import :Float4;
import :Float4x4;
import :Quaternion;
import CopiumEngine.Core.CoreTypes;


export namespace Copium::Math
{

    // NOTE(v.matushkin): Probably doesn't belong here, just don't know where to put these
    [[nodiscard]] constexpr Float3 Up()      noexcept { return Float3( 0.0f,  1.0f,  0.0f); }
    [[nodiscard]] constexpr Float3 Down()    noexcept { return Float3( 0.0f, -1.0f,  0.0f); }
    [[nodiscard]] constexpr Float3 Forward() noexcept { return Float3( 0.0f,  0.0f,  1.0f); }
    [[nodiscard]] constexpr Float3 Back()    noexcept { return Float3( 0.0f,  0.0f, -1.0f); }
    // TODO(v.matushkin): Not sure why my Left and Right are the opposite of what they're supposed to be
    [[nodiscard]] constexpr Float3 Left()    noexcept { return Float3( 1.0f,  0.0f,  0.0f); }
    [[nodiscard]] constexpr Float3 Right()   noexcept { return Float3(-1.0f,  0.0f,  0.0f); }


    [[nodiscard]] constexpr Float4x4 Translate(const Float3& translation) noexcept
    {
        return Float4x4(0.0f, 0.0f, 0.0f, translation.X,
                        0.0f, 0.0f, 0.0f, translation.Y,
                        0.0f, 0.0f, 0.0f, translation.Z,
                        0.0f, 0.0f, 0.0f,          1.0f);
    }

    [[nodiscard]] constexpr Float4x4 Scale(float32 scale) noexcept
    {
        return Float4x4(scale,  0.0f,  0.0f, 0.0f,
                         0.0f, scale,  0.0f, 0.0f,
                         0.0f,  0.0f, scale, 0.0f,
                         0.0f,  0.0f,  0.0f, 1.0f);
    }

    [[nodiscard]] constexpr Float4x4 Scale(const Float3& scale) noexcept
    {
        return Float4x4(scale.X,    0.0f,    0.0f, 0.0f,
                           0.0f, scale.Y,    0.0f, 0.0f,
                           0.0f,    0.0f, scale.Z, 0.0f,
                           0.0f,    0.0f,    0.0f, 1.0f);
    }

    [[nodiscard]] constexpr Float4x4 TR(const Float3& translation, const Quaternion& rotation) noexcept
    {
        Float3x3 r = rotation.ToFloat3x3();

        return Float4x4(Float4(r.C0, 0.0f),
                        Float4(r.C1, 0.0f),
                        Float4(r.C2, 0.0f),
                        Float4(r * translation, 1.0f));
    }

    [[nodiscard]] constexpr Float4x4 TRS(const Float3& translation, const Quaternion& rotation, const Float3& scale) noexcept
    {
        Float3x3 r = rotation.ToFloat3x3();

        return Float4x4(Float4(r.C0 * scale.X, 0.0f),
                        Float4(r.C1 * scale.Y, 0.0f),
                        Float4(r.C2 * scale.Z, 0.0f),
                        Float4(r * translation, 1.0f));
    }

} // export namespace Copium::Math
