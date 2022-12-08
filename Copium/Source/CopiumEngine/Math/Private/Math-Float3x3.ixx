export module CopiumEngine.Math:Float3x3;

import :Float3;
import :Functions;

import Copium.Core;


export namespace Copium
{

    struct Float3x3 final
    {
        Float3 C0;
        Float3 C1;
        Float3 C2;


        constexpr Float3x3() noexcept = default;
        constexpr Float3x3(const Float3x3& other) noexcept = default;

        explicit constexpr Float3x3(float32 scalar) noexcept;
        constexpr Float3x3(const Float3& c0, const Float3& c1, const Float3& c2) noexcept;
        constexpr Float3x3(float32 m00, float32 m01, float32 m02,
                           float32 m10, float32 m11, float32 m12,
                           float32 m20, float32 m21, float32 m22) noexcept;

        //- Compound
        //-- Vector | Scalar
        constexpr Float3x3& operator+=(float32 rhs) noexcept { C0 += rhs; C1 += rhs; C2 += rhs; return *this; }
        constexpr Float3x3& operator-=(float32 rhs) noexcept { C0 -= rhs; C1 -= rhs; C2 -= rhs; return *this; }
        constexpr Float3x3& operator*=(float32 rhs) noexcept { C0 *= rhs; C1 *= rhs; C2 *= rhs; return *this; }
        //-- Vector | Vector
        constexpr Float3x3& operator+=(const Float3x3& rhs) noexcept { C0 += rhs.C0; C1 += rhs.C1; C2 += rhs.C2; return *this; }
        constexpr Float3x3& operator-=(const Float3x3& rhs) noexcept { C0 -= rhs.C0; C1 -= rhs.C1; C2 -= rhs.C2; return *this; }
        constexpr Float3x3& operator*=(const Float3x3& rhs) noexcept { C0 *= rhs.C0; C1 *= rhs.C1; C2 *= rhs.C2; return *this; }

        //- Unary
        friend constexpr Float3x3 operator-(const Float3x3& rhs) noexcept { return Float3x3(-rhs.C0, -rhs.C1, -rhs.C2); }

        //- Binary
        //-- Matrix | Scalar
        friend constexpr Float3x3 operator+(const Float3x3& lhs, float32 rhs) noexcept { return Float3x3(lhs.C0 + rhs, lhs.C1 + rhs, lhs.C2 + rhs); }
        friend constexpr Float3x3 operator-(const Float3x3& lhs, float32 rhs) noexcept { return Float3x3(lhs.C0 - rhs, lhs.C1 - rhs, lhs.C2 - rhs); }
        friend constexpr Float3x3 operator*(const Float3x3& lhs, float32 rhs) noexcept { return Float3x3(lhs.C0 * rhs, lhs.C1 * rhs, lhs.C2 * rhs); }
        //-- Scalar | Matrix
        friend constexpr Float3x3 operator+(float32 lhs, const Float3x3& rhs) noexcept { return Float3x3(lhs + rhs.C0, lhs + rhs.C1, lhs + rhs.C2); }
        friend constexpr Float3x3 operator-(float32 lhs, const Float3x3& rhs) noexcept { return Float3x3(lhs - rhs.C0, lhs - rhs.C1, lhs - rhs.C2); }
        friend constexpr Float3x3 operator*(float32 lhs, const Float3x3& rhs) noexcept { return Float3x3(lhs * rhs.C0, lhs * rhs.C1, lhs * rhs.C2); }
        //-- Matrix | Matrix
        friend constexpr Float3x3 operator+(const Float3x3& lhs, const Float3x3& rhs) noexcept { return Float3x3(lhs.C0 + rhs.C0, lhs.C1 + rhs.C1, lhs.C2 + rhs.C2); }
        friend constexpr Float3x3 operator-(const Float3x3& lhs, const Float3x3& rhs) noexcept { return Float3x3(lhs.C0 - rhs.C0, lhs.C1 - rhs.C1, lhs.C2 - rhs.C2); }
        friend constexpr Float3x3 operator*(const Float3x3& lhs, const Float3x3& rhs) noexcept { return Float3x3(lhs.C0 * rhs.C0, lhs.C1 * rhs.C1, lhs.C2 * rhs.C2); }

        [[nodiscard]] constexpr float32 Determinant() const noexcept;
        [[nodiscard]] constexpr Float3x3 Transpose() const noexcept;
        [[nodiscard]] constexpr Float3x3 Inverse() const noexcept;

        [[nodiscard]] static constexpr Float3x3 Zero() noexcept { return Float3x3(0.0f); }
        [[nodiscard]] static constexpr Float3x3 Identity() noexcept;

        //- Transform
        [[nodiscard]] static inline Float3x3 RotateX(float32 angle) noexcept;
        [[nodiscard]] static inline Float3x3 RotateY(float32 angle) noexcept;
        [[nodiscard]] static inline Float3x3 RotateZ(float32 angle) noexcept;
        [[nodiscard]] static constexpr Float3x3 Scale(float32 scale) noexcept;
        [[nodiscard]] static constexpr Float3x3 Scale(float32 scaleX, float32 scaleY, float32 scaleZ) noexcept;
        [[nodiscard]] static constexpr Float3x3 Scale(const Float3& scale) noexcept;
    };


    constexpr Float3x3::Float3x3(float32 scalar) noexcept
        : C0(scalar)
        , C1(scalar)
        , C2(scalar)
    {}

    constexpr Float3x3::Float3x3(const Float3& c0, const Float3& c1, const Float3& c2) noexcept
        : C0(c0)
        , C1(c1)
        , C2(c2)
    {}

    constexpr Float3x3::Float3x3(float32 m00, float32 m01, float32 m02,
                                 float32 m10, float32 m11, float32 m12,
                                 float32 m20, float32 m21, float32 m22) noexcept
        : C0(m00, m10, m20)
        , C1(m01, m11, m21)
        , C2(m02, m12, m22)
    {}


    constexpr float32 Float3x3::Determinant() const noexcept
    {
        const float32 d1 = C1.Y * C2.Z - C1.Z * C2.Y;
        const float32 d2 = C0.Y * C2.Z - C0.Z * C2.Y;
        const float32 d3 = C0.Y * C1.Z - C0.Z * C1.Y;

        return C0.X * d1 - C1.X * d2 + C2.X * d3;
    }

    constexpr Float3x3 Float3x3::Transpose() const noexcept
    {
        return Float3x3(C0.X, C0.Y, C0.Z,
                        C1.X, C1.Y, C1.Z,
                        C2.X, C2.Y, C2.Z);
    }

    // TODO(v.matushkin): Determinant probably can be computed more efficiently
    constexpr Float3x3 Float3x3::Inverse() const noexcept
    {
        const float32 m00 = C1.Y * C2.Z - C1.Z * C2.Y;
        const float32 m10 = C1.Z * C2.X - C1.X * C2.Z;
        const float32 m20 = C1.X * C2.Y - C1.Y * C2.X;

        const float32 m01 = C0.Z * C2.Y - C0.Y * C2.Z;
        const float32 m11 = C0.X * C2.Z - C0.Z * C2.X;
        const float32 m21 = C0.Y * C2.X - C0.X * C2.Y;

        const float32 m02 = C0.Y * C1.Z - C0.Z * C1.Y;
        const float32 m12 = C0.Z * C1.X - C0.X * C1.Z;
        const float32 m22 = C0.X * C1.Y - C0.Y * C1.X;

        const float32 rcpDeterminant = 1.0f / Determinant();

        return Float3x3(m00 * rcpDeterminant, m10 * rcpDeterminant, m20 * rcpDeterminant,
                        m01 * rcpDeterminant, m11 * rcpDeterminant, m21 * rcpDeterminant,
                        m02 * rcpDeterminant, m12 * rcpDeterminant, m22 * rcpDeterminant);
    }


    constexpr Float3x3 Float3x3::Identity() noexcept
    {
        return Float3x3(1.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 1.0f);
    }


    Float3x3 Float3x3::RotateX(float32 angle) noexcept
    {
        const auto [sin, cos] = Math::SinCos(angle);
        return Float3x3(1.0f, 0.0f, 0.0f,
                        0.0f,  cos, -sin,
                        0.0f,  sin,  cos);
    }

    Float3x3 Float3x3::RotateY(float32 angle) noexcept
    {
        const auto [sin, cos] = Math::SinCos(angle);
        return Float3x3( cos, 0.0f,  sin,
                        0.0f, 1.0f, 0.0f,
                        -sin, 0.0f,  cos);
    }

    Float3x3 Float3x3::RotateZ(float32 angle) noexcept
    {
        const auto [sin, cos] = Math::SinCos(angle);
        return Float3x3( cos, -sin, 0.0f,
                         sin,  cos, 0.0f,
                        0.0f, 0.0f, 1.0f);
    }

    constexpr Float3x3 Float3x3::Scale(float32 scale) noexcept
    {
        return Float3x3(scale,  0.0f,  0.0f,
                         0.0f, scale,  0.0f,
                         0.0f,  0.0f, scale);
    }

    constexpr Float3x3 Float3x3::Scale(float32 scaleX, float32 scaleY, float32 scaleZ) noexcept
    {
        return Float3x3(scaleX,   0.0f,   0.0f,
                          0.0f, scaleY,   0.0f,
                          0.0f,   0.0f, scaleZ);
    }

    constexpr Float3x3 Float3x3::Scale(const Float3& scale) noexcept
    {
        return Float3x3(scale.X,    0.0f,    0.0f,
                           0.0f, scale.Y,    0.0f,
                           0.0f,    0.0f, scale.Z);
    }

} // export namespace Copium
