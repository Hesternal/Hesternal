export module CopiumEngine.Math:Transform;

import :Float3;
import :Float4;
import :Float3x3;
import :Float4x4;
import :Mul;
import :Quaternion;
import CopiumEngine.Core.CoreTypes;


export namespace Copium::Math
{

    // NOTE(v.matushkin): Probably doesn't belong here, just don't know where to put these
    [[nodiscard]] constexpr Float3 Up()      noexcept { return Float3( 0.0f,  1.0f,  0.0f); }
    [[nodiscard]] constexpr Float3 Down()    noexcept { return Float3( 0.0f, -1.0f,  0.0f); }
    [[nodiscard]] constexpr Float3 Forward() noexcept { return Float3( 0.0f,  0.0f,  1.0f); }
    [[nodiscard]] constexpr Float3 Back()    noexcept { return Float3( 0.0f,  0.0f, -1.0f); }
    [[nodiscard]] constexpr Float3 Left()    noexcept { return Float3(-1.0f,  0.0f,  0.0f); }
    [[nodiscard]] constexpr Float3 Right()   noexcept { return Float3( 1.0f,  0.0f,  0.0f); }


    [[nodiscard]] constexpr Float4x4 TR(const Float3& translation, const Float3x3& rotation) noexcept
    {
        return Float4x4(Float4(rotation.C0, 0.0f),
                        Float4(rotation.C1, 0.0f),
                        Float4(rotation.C2, 0.0f),
                        Float4(translation, 1.0f));
    }

    [[nodiscard]] constexpr Float4x4 TR(const Float3& translation, const Quaternion& rotation) noexcept
    {
        const Float3x3 r = rotation.ToFloat3x3();

        return Float4x4(Float4(r.C0, 0.0f),
                        Float4(r.C1, 0.0f),
                        Float4(r.C2, 0.0f),
                        Float4(translation, 1.0f));
    }

    [[nodiscard]] constexpr Float4x4 TRS(const Float3& translation, const Quaternion& rotation, const Float3& scale) noexcept
    {
        const Float3x3 r = rotation.ToFloat3x3();

        return Float4x4(Float4(r.C0 * scale.X, 0.0f),
                        Float4(r.C1 * scale.Y, 0.0f),
                        Float4(r.C2 * scale.Z, 0.0f),
                        Float4(translation, 1.0f));
    }


    /// For Rotation + Translation
    [[nodiscard]] constexpr Float4x4 InverseRigidTransform(const Float4x4& m) noexcept
    {
        // Inverse of Rotation matrix is Transpose
        // NOTE(v.matushkin): Not sure about reflection
        const Float3x3 invLinear = Float3x3(m.C0.X, m.C0.Y, m.C0.Z,
                                            m.C1.X, m.C1.Y, m.C1.Z,
                                            m.C2.X, m.C2.Y, m.C2.Z);

        const Float3 invTranslation = -Mul(invLinear, Float3(m.C3.X, m.C3.Y, m.C3.Z));

        return Float4x4(invLinear.C0.X, invLinear.C1.X, invLinear.C2.X, invTranslation.X,
                        invLinear.C0.Y, invLinear.C1.Y, invLinear.C2.Y, invTranslation.Y,
                        invLinear.C0.Z, invLinear.C1.Z, invLinear.C2.Z, invTranslation.Z,
                                  0.0f,           0.0f,           0.0f,             1.0f);
    }

    /// For Linear Transform + Translation
    [[nodiscard]] constexpr Float4x4 InverseAffineTransform(const Float4x4& m) noexcept
    {
        const Float3x3 invLinear = Float3x3(m.C0.X, m.C1.X, m.C2.X,
                                            m.C0.Y, m.C1.Y, m.C2.Y,
                                            m.C0.Z, m.C1.Z, m.C2.Z).Inverse();

        const Float3 invTranslation = -Mul(invLinear, Float3(m.C3.X, m.C3.Y, m.C3.Z));

        return Float4x4(invLinear.C0.X, invLinear.C1.X, invLinear.C2.X, invTranslation.X,
                        invLinear.C0.Y, invLinear.C1.Y, invLinear.C2.Y, invTranslation.Y,
                        invLinear.C0.Z, invLinear.C1.Z, invLinear.C2.Z, invTranslation.Z,
                                  0.0f,           0.0f,           0.0f,             1.0f);
    }

} // export namespace Copium::Math
