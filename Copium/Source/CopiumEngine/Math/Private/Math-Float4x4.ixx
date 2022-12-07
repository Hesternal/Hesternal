export module CopiumEngine.Math:Float4x4;

import :Float4;
import :Functions;
import CopiumEngine.Core.CoreTypes;


export namespace Copium
{

    struct Float4x4 final
    {
        Float4 C0;
        Float4 C1;
        Float4 C2;
        Float4 C3;


        constexpr Float4x4() noexcept = default;
        constexpr Float4x4(const Float4x4& other) noexcept = default;

        explicit constexpr Float4x4(float32 scalar) noexcept;
        constexpr Float4x4(const Float4& c0, const Float4& c1, const Float4& c2, const Float4& c3) noexcept;
        constexpr Float4x4(float32 m00, float32 m01, float32 m02, float32 m03,
                           float32 m10, float32 m11, float32 m12, float32 m13,
                           float32 m20, float32 m21, float32 m22, float32 m23,
                           float32 m30, float32 m31, float32 m32, float32 m33) noexcept;

        //- Compound
        //-- Vector | Scalar
        constexpr Float4x4& operator+=(float32 rhs) noexcept { C0 += rhs; C1 += rhs; C2 += rhs; C3 += rhs; return *this; }
        constexpr Float4x4& operator-=(float32 rhs) noexcept { C0 -= rhs; C1 -= rhs; C2 -= rhs; C3 -= rhs; return *this; }
        constexpr Float4x4& operator*=(float32 rhs) noexcept { C0 *= rhs; C1 *= rhs; C2 *= rhs; C3 *= rhs; return *this; }
        //-- Vector | Vector
        constexpr Float4x4& operator+=(const Float4x4& rhs) noexcept { C0 += rhs.C0; C1 += rhs.C1; C2 += rhs.C2; C3 += rhs.C3; return *this; }
        constexpr Float4x4& operator-=(const Float4x4& rhs) noexcept { C0 -= rhs.C0; C1 -= rhs.C1; C2 -= rhs.C2; C3 -= rhs.C3; return *this; }
        constexpr Float4x4& operator*=(const Float4x4& rhs) noexcept { C0 *= rhs.C0; C1 *= rhs.C1; C2 *= rhs.C2; C3 *= rhs.C3; return *this; }

        //- Unary
        friend constexpr Float4x4 operator-(const Float4x4& rhs) noexcept { return Float4x4(-rhs.C0, -rhs.C1, -rhs.C2, -rhs.C3); }

        //- Binary
        //-- Matrix | Scalar
        friend constexpr Float4x4 operator+(const Float4x4& lhs, float32 rhs) noexcept { return Float4x4(lhs.C0 + rhs, lhs.C1 + rhs, lhs.C2 + rhs, lhs.C3 + rhs); }
        friend constexpr Float4x4 operator-(const Float4x4& lhs, float32 rhs) noexcept { return Float4x4(lhs.C0 - rhs, lhs.C1 - rhs, lhs.C2 - rhs, lhs.C3 - rhs); }
        friend constexpr Float4x4 operator*(const Float4x4& lhs, float32 rhs) noexcept { return Float4x4(lhs.C0 * rhs, lhs.C1 * rhs, lhs.C2 * rhs, lhs.C3 * rhs); }
        //-- Scalar | Matrix
        friend constexpr Float4x4 operator+(float32 lhs, const Float4x4& rhs) noexcept { return Float4x4(lhs + rhs.C0, lhs + rhs.C1, lhs + rhs.C2, lhs + rhs.C3); }
        friend constexpr Float4x4 operator-(float32 lhs, const Float4x4& rhs) noexcept { return Float4x4(lhs - rhs.C0, lhs - rhs.C1, lhs - rhs.C2, lhs - rhs.C3); }
        friend constexpr Float4x4 operator*(float32 lhs, const Float4x4& rhs) noexcept { return Float4x4(lhs * rhs.C0, lhs * rhs.C1, lhs * rhs.C2, lhs * rhs.C3); }
        //-- Matrix | Matrix
        friend constexpr Float4x4 operator+(const Float4x4& lhs, const Float4x4& rhs) noexcept { return Float4x4(lhs.C0 + rhs.C0, lhs.C1 + rhs.C1, lhs.C2 + rhs.C2, lhs.C3 + rhs.C3); }
        friend constexpr Float4x4 operator-(const Float4x4& lhs, const Float4x4& rhs) noexcept { return Float4x4(lhs.C0 - rhs.C0, lhs.C1 - rhs.C1, lhs.C2 - rhs.C2, lhs.C3 - rhs.C3); }
        friend constexpr Float4x4 operator*(const Float4x4& lhs, const Float4x4& rhs) noexcept { return Float4x4(lhs.C0 * rhs.C0, lhs.C1 * rhs.C1, lhs.C2 * rhs.C2, lhs.C3 * rhs.C3); }

        [[nodiscard]] constexpr float32 Determinant() const noexcept;
        [[nodiscard]] constexpr Float4x4 Transpose() const noexcept;
        // TODO(v.matushkin): Implement
        // [[nodiscard]] constexpr Float4x4 Inverse() const noexcept;

        [[nodiscard]] static constexpr Float4x4 Zero() noexcept { return Float4x4(0.0f); }
        [[nodiscard]] static constexpr Float4x4 Identity() noexcept;

        [[nodiscard]] static constexpr Float4x4 Translate(float32 translation) noexcept;
        [[nodiscard]] static constexpr Float4x4 Translate(float32 translationX, float32 translationY, float32 translationZ) noexcept;
        [[nodiscard]] static constexpr Float4x4 Translate(const Float3& translation) noexcept;
        [[nodiscard]] static inline Float4x4 RotateX(float32 angle) noexcept;
        [[nodiscard]] static inline Float4x4 RotateY(float32 angle) noexcept;
        [[nodiscard]] static inline Float4x4 RotateZ(float32 angle) noexcept;
        [[nodiscard]] static constexpr Float4x4 Scale(float32 scale) noexcept;
        [[nodiscard]] static constexpr Float4x4 Scale(float32 scaleX, float32 scaleY, float32 scaleZ) noexcept;
        [[nodiscard]] static constexpr Float4x4 Scale(const Float3& scale) noexcept;
    };


    constexpr Float4x4::Float4x4(float32 scalar) noexcept
        : C0(scalar)
        , C1(scalar)
        , C2(scalar)
        , C3(scalar)
    {}

    constexpr Float4x4::Float4x4(const Float4& c0, const Float4& c1, const Float4& c2, const Float4& c3) noexcept
        : C0(c0)
        , C1(c1)
        , C2(c2)
        , C3(c3)
    {}

    //< HAND FIXED
    constexpr Float4x4::Float4x4(float32 m00, float32 m01, float32 m02, float32 m03,
                                 float32 m10, float32 m11, float32 m12, float32 m13,
                                 float32 m20, float32 m21, float32 m22, float32 m23,
                                 float32 m30, float32 m31, float32 m32, float32 m33) noexcept
        : C0(m00, m10, m20, m30)
        , C1(m01, m11, m21, m31)
        , C2(m02, m12, m22, m32)
        , C3(m03, m13, m23, m33)
    {}
    //> HAND FIXED


    constexpr float32 Float4x4::Determinant() const noexcept
    {
        const float32 d1 = C1.Y * (C2.Z * C3.W - C2.W * C3.Z) - C2.Y * (C1.Z * C3.W - C1.W * C3.Z) + C3.Y * (C1.Z * C2.W - C1.W * C2.Z);
        const float32 d2 = C0.Y * (C2.Z * C3.W - C2.W * C3.Z) - C2.Y * (C0.Z * C3.W - C0.W * C3.Z) + C3.Y * (C0.Z * C2.W - C0.W * C2.Z);
        const float32 d3 = C0.Y * (C1.Z * C3.W - C1.W * C3.Z) - C1.Y * (C0.Z * C3.W - C0.W * C3.Z) + C3.Y * (C0.Z * C1.W - C0.W * C1.Z);
        const float32 d4 = C0.Y * (C1.Z * C2.W - C1.W * C2.Z) - C1.Y * (C0.Z * C2.W - C0.W * C2.Z) + C2.Y * (C0.Z * C1.W - C0.W * C1.Z);

        return C0.X * d1 - C1.X * d2 + C2.X * d3 - C3.X * d4;
    }

    constexpr Float4x4 Float4x4::Transpose() const noexcept
    {
        return Float4x4(C0.X, C0.Y, C0.Z, C0.W,
                        C1.X, C1.Y, C1.Z, C1.W,
                        C2.X, C2.Y, C2.Z, C2.W,
                        C3.X, C3.Y, C3.Z, C3.W);
    }


    constexpr Float4x4 Float4x4::Identity() noexcept
    {
        return Float4x4(1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);
    }


    constexpr Float4x4 Float4x4::Translate(float32 translation) noexcept
    {
        return Float4x4(1.0f, 0.0f, 0.0f, translation,
                        0.0f, 1.0f, 0.0f, translation,
                        0.0f, 0.0f, 1.0f, translation,
                        0.0f, 0.0f, 0.0f,        1.0f);
    }

    constexpr Float4x4 Float4x4::Translate(float32 translationX, float32 translationY, float32 translationZ) noexcept
    {
        return Float4x4(1.0f, 0.0f, 0.0f, translationX,
                        0.0f, 1.0f, 0.0f, translationY,
                        0.0f, 0.0f, 1.0f, translationZ,
                        0.0f, 0.0f, 0.0f,         1.0f);
    }

    constexpr Float4x4 Float4x4::Translate(const Float3& translation) noexcept
    {
        return Float4x4(1.0f, 0.0f, 0.0f, translation.X,
                        0.0f, 1.0f, 0.0f, translation.Y,
                        0.0f, 0.0f, 1.0f, translation.Z,
                        0.0f, 0.0f, 0.0f,          1.0f);
    }

    Float4x4 Float4x4::RotateX(float32 angle) noexcept
    {
        const auto [sin, cos] = Math::SinCos(angle);
        return Float4x4(1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f,  cos, -sin, 0.0f,
                        0.0f,  sin,  cos, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);
    }

    Float4x4 Float4x4::RotateY(float32 angle) noexcept
    {
        const auto [sin, cos] = Math::SinCos(angle);
        return Float4x4( cos, 0.0f,  sin, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        -sin, 0.0f,  cos, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);
    }

    Float4x4 Float4x4::RotateZ(float32 angle) noexcept
    {
        const auto [sin, cos] = Math::SinCos(angle);
        return Float4x4( cos, -sin, 0.0f, 0.0f,
                         sin,  cos, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);
    }

    constexpr Float4x4 Float4x4::Scale(float32 scale) noexcept
    {
        return Float4x4(scale,  0.0f,  0.0f, 0.0f,
                         0.0f, scale,  0.0f, 0.0f,
                         0.0f,  0.0f, scale, 0.0f,
                         0.0f,  0.0f,  0.0f, 1.0f);
    }

    constexpr Float4x4 Float4x4::Scale(float32 scaleX, float32 scaleY, float32 scaleZ) noexcept
    {
        return Float4x4(scaleX,   0.0f,   0.0f, 0.0f,
                          0.0f, scaleY,   0.0f, 0.0f,
                          0.0f,   0.0f, scaleZ, 0.0f,
                          0.0f,   0.0f,   0.0f, 1.0f);
    }

    constexpr Float4x4 Float4x4::Scale(const Float3& scale) noexcept
    {
        return Float4x4(scale.X,    0.0f,    0.0f, 0.0f,
                           0.0f, scale.Y,    0.0f, 0.0f,
                           0.0f,    0.0f, scale.Z, 0.0f,
                           0.0f,    0.0f,    0.0f, 1.0f);
    }

} // export namespace Copium
