export module CopiumEngine.Math:Types;

import :Constants;
import :Functions;
import CopiumEngine.Core.CoreTypes;

import <cmath>;


export namespace Copium
{

    struct Float3x3;
    struct Float4x4;
    struct Quaternion;


    struct Float2 final
    {
        float32 X, Y;

        constexpr Float2() = default;
        constexpr Float2(const Float2& v) = default;

        explicit constexpr Float2(float32 scalar);
        constexpr Float2(float32 x, float32 y);
    };

    struct Double2 final
    {
        float64 X, Y;

        constexpr Double2() = default;
        constexpr Double2(const Double2& v) = default;

        explicit constexpr Double2(float64 scalar);
        constexpr Double2(float64 x, float64 y);
    };


    struct Float3 final
    {
        float32 X, Y, Z;


        constexpr Float3() = default;
        constexpr Float3(const Float3& v) = default;

        explicit constexpr Float3(float32 scalar);
        constexpr Float3(float32 x, float32 y, float32 z);

        constexpr Float3& operator+=(float32 rhs);
        constexpr Float3& operator-=(float32 rhs);
        constexpr Float3& operator*=(float32 rhs);
        constexpr Float3& operator+=(const Float3& rhs);
        constexpr Float3& operator-=(const Float3& rhs);
        constexpr Float3& operator*=(const Float3& rhs);

        [[nodiscard]] static constexpr Float3 Zero() { return Float3(0.0f); }
    };


    struct Float4 final
    {
        float32 X, Y, Z, W;


        constexpr Float4() = default;
        constexpr Float4(const Float4& v) = default;

        explicit constexpr Float4(float32 scalar);
        constexpr Float4(float32 x, float32 y, float32 z, float32 w);
        constexpr Float4(const Float3& xyz, float32 w);

        constexpr Float4& operator+=(float32 rhs);
        constexpr Float4& operator-=(float32 rhs);
        constexpr Float4& operator*=(float32 rhs);
        constexpr Float4& operator+=(const Float4& rhs);
        constexpr Float4& operator-=(const Float4& rhs);
        constexpr Float4& operator*=(const Float4& rhs);

        [[nodiscard]] static constexpr Float4 Zero() { return Float4(0.0f); }
    };


    /// Column major
    struct Float3x3 final
    {
        Float3 C0, C1, C2;


        constexpr Float3x3() = default;
        constexpr Float3x3(const Float3x3& m) = default;

        constexpr Float3x3(float32 m00, float32 m01, float32 m02,
                           float32 m10, float32 m11, float32 m12,
                           float32 m20, float32 m21, float32 m22);
        constexpr Float3x3(const Float3& c0, const Float3& c1, const Float3& c2);
        constexpr Float3x3(const Quaternion& q);

        constexpr Float3x3& operator+=(const Float3x3& rhs);
        constexpr Float3x3& operator-=(const Float3x3& rhs);
        constexpr Float3x3& operator*=(const Float3x3& rhs);
    };


    /// Column major
    struct Float4x4 final
    {
        Float4 C0, C1, C2, C3;


        constexpr Float4x4() = default;
        constexpr Float4x4(const Float4x4& m) = default;

        constexpr Float4x4(float32 m00, float32 m01, float32 m02, float32 m03,
                           float32 m10, float32 m11, float32 m12, float32 m13,
                           float32 m20, float32 m21, float32 m22, float32 m23,
                           float32 m30, float32 m31, float32 m32, float32 m33);
        constexpr Float4x4(const Float4& c0, const Float4& c1, const Float4& c2, const Float4& c3);

        constexpr Float4x4& operator+=(const Float4x4& rhs);
        constexpr Float4x4& operator-=(const Float4x4& rhs);
        constexpr Float4x4& operator*=(const Float4x4& rhs);

        [[nodiscard]] static constexpr Float4x4 Zero();
        [[nodiscard]] static constexpr Float4x4 Identity();

        [[nodiscard]] static constexpr Float4x4 Translate(const Float3& translation);
        [[nodiscard]] static constexpr Float4x4 Scale(float32 scale);
        [[nodiscard]] static constexpr Float4x4 Scale(const Float3& scale);
        [[nodiscard]] static constexpr Float4x4 TRS(const Float3& translation, const Quaternion& rotation, const Float3& scale);

        /// verticalFov in radians
        [[nodiscard]] static Float4x4 Perspective(float32 verticalFov, float32 aspect, float32 near, float32 far);
    };


    struct Quaternion final
    {
        float32 X, Y, Z, W;


        constexpr Quaternion() = default;
        constexpr Quaternion(const Quaternion& v) = default;

        constexpr Quaternion(float32 x, float32 y, float32 z, float32 w);
        constexpr Quaternion(const Float3& xyz, float32 w);
        constexpr Quaternion(const Float4& value);

        /// <summary>Returns the result of transforming the quaternion b by the quaternion a.</summary>
        [[nodiscard]] static constexpr Quaternion Mul(const Quaternion& lhs, const Quaternion& rhs);
        /// <summary>Returns the result of rotating a vector by a unit quaternion.</summary>
        [[nodiscard]] static constexpr Float3 Mul(const Quaternion& lhs, const Float3& rhs);

        [[nodiscard]] static constexpr Quaternion Identity() { return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); }

        /// <param name="axis">The unit axis of rotation.</param>
        /// <param name="angle">The angle of rotation in radians( ARE YOU SURE? ).</param>
        [[nodiscard]] static Quaternion AxisAngle(const Float3& axis, float32 angle);
    };

} // export namespace Copium


export namespace Copium::Math
{

    [[nodiscard]] constexpr Float3 Up()      { return Float3( 0.0f,  1.0f,  0.0f); }
    [[nodiscard]] constexpr Float3 Down()    { return Float3( 0.0f, -1.0f,  0.0f); }
    [[nodiscard]] constexpr Float3 Forward() { return Float3( 0.0f,  0.0f,  1.0f); }
    [[nodiscard]] constexpr Float3 Back()    { return Float3( 0.0f,  0.0f, -1.0f); }
    [[nodiscard]] constexpr Float3 Left()    { return Float3(-1.0f,  0.0f,  0.0f); }
    [[nodiscard]] constexpr Float3 Right()   { return Float3( 1.0f,  0.0f,  0.0f); }

    [[nodiscard]] constexpr Float3 Radians(const Float3& degrees);
    [[nodiscard]] constexpr Float4 Radians(const Float4& degrees);

    [[nodiscard]] constexpr Float3 Degrees(const Float3& radians);
    [[nodiscard]] constexpr Float4 Degrees(const Float4& radians);

    [[nodiscard]] constexpr float32 Dot(const Float3& lhs, const Float3& rhs);
    [[nodiscard]] constexpr float32 Dot(const Float4& lhs, const Float4& rhs);

    [[nodiscard]] constexpr Float3 Cross(const Float3& lhs, const Float3& rhs);

} // export namespace Copium::Math


export namespace Copium
{

    // ========================================================================
    // ================================ Float2 ================================
    // ========================================================================

    constexpr Float2::Float2(float32 scalar)
        : X(scalar)
        , Y(scalar)
    {
    }

    constexpr Float2::Float2(float32 x, float32 y)
        : X(x)
        , Y(y)
    {
    }


    // ========================================================================
    // =============================== Double2 ================================
    // ========================================================================

    constexpr Double2::Double2(float64 scalar)
        : X(scalar)
        , Y(scalar)
    {
    }

    constexpr Double2::Double2(float64 x, float64 y)
        : X(x)
        , Y(y)
    {
    }


    // ========================================================================
    // ================================ Float3 ================================
    // ========================================================================

    constexpr Float3::Float3(float32 scalar)
        : X(scalar)
        , Y(scalar)
        , Z(scalar)
    {
    }

    constexpr Float3::Float3(float32 x, float32 y, float32 z)
        : X(x)
        , Y(y)
        , Z(z)
    {
    }


    //- Unary

    constexpr Float3 operator-(const Float3& value) { return Float3(-value.X, -value.Y, -value.Z); }

    //- Compound

    //-- Vector | Scalar
    constexpr Float3& Float3::operator+=(float32 rhs) { X += rhs; Y += rhs; Z += rhs; return *this; }
    constexpr Float3& Float3::operator-=(float32 rhs) { X -= rhs; Y -= rhs; Z -= rhs; return *this; }
    constexpr Float3& Float3::operator*=(float32 rhs) { X *= rhs; Y *= rhs; Z *= rhs; return *this; }
    //-- Vector | Vector
    constexpr Float3& Float3::operator+=(const Float3& rhs) { X += rhs.X; Y += rhs.Y; Z += rhs.Z;  return *this; }
    constexpr Float3& Float3::operator-=(const Float3& rhs) { X -= rhs.X; Y -= rhs.Y; Z -= rhs.Z;  return *this; }
    constexpr Float3& Float3::operator*=(const Float3& rhs) { X *= rhs.X; Y *= rhs.Y; Z *= rhs.Z;  return *this; }

    //- Binary

    //-- Vector | Scalar
    constexpr Float3 operator+(const Float3& lhs, float32 rhs) { return Float3(lhs.X + rhs, lhs.Y + rhs, lhs.Z + rhs); }
    constexpr Float3 operator-(const Float3& lhs, float32 rhs) { return Float3(lhs.X - rhs, lhs.Y - rhs, lhs.Z - rhs); }
    constexpr Float3 operator*(const Float3& lhs, float32 rhs) { return Float3(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs); }

    //-- Scalar | Vector
    constexpr Float3 operator+(float32 lhs, const Float3& rhs) { return Float3(lhs + rhs.X, lhs + rhs.Y, lhs + rhs.Z); }
    constexpr Float3 operator-(float32 lhs, const Float3& rhs) { return Float3(lhs - rhs.X, lhs - rhs.Y, lhs - rhs.Z); }
    constexpr Float3 operator*(float32 lhs, const Float3& rhs) { return Float3(lhs * rhs.X, lhs * rhs.Y, lhs * rhs.Z); }

    //-- Vector | Vector
    constexpr Float3 operator+(const Float3& lhs, const Float3& rhs) { return Float3(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z); }
    constexpr Float3 operator-(const Float3& lhs, const Float3& rhs) { return Float3(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z); }
    constexpr Float3 operator*(const Float3& lhs, const Float3& rhs) { return Float3(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z); }


    // ========================================================================
    // ================================ Float4 ================================
    // ========================================================================

    constexpr Float4::Float4(float32 scalar)
        : X(scalar)
        , Y(scalar)
        , Z(scalar)
        , W(scalar)
    {
    }

    constexpr Float4::Float4(float32 x, float32 y, float32 z, float32 w)
        : X(x)
        , Y(y)
        , Z(z)
        , W(w)
    {
    }

    constexpr Float4::Float4(const Float3& xyz, float32 w)
        : X(xyz.X)
        , Y(xyz.Y)
        , Z(xyz.Z)
        , W(w)
    {
    }


    //- Unary

    constexpr Float4 operator-(const Float4& value) { return Float4(-value.X, -value.Y, -value.Z, -value.W); }

    //- Compound

    //-- Vector | Scalar
    constexpr Float4& Float4::operator+=(float32 rhs) { X += rhs; Y += rhs; Z += rhs; W += rhs; return *this; }
    constexpr Float4& Float4::operator-=(float32 rhs) { X -= rhs; Y -= rhs; Z -= rhs; W -= rhs; return *this; }
    constexpr Float4& Float4::operator*=(float32 rhs) { X *= rhs; Y *= rhs; Z *= rhs; W *= rhs; return *this; }

    //-- Vector | Vector
    constexpr Float4& Float4::operator+=(const Float4& rhs) { X += rhs.X; Y += rhs.Y; Z += rhs.Z; W += rhs.W; return *this; }
    constexpr Float4& Float4::operator-=(const Float4& rhs) { X -= rhs.X; Y -= rhs.Y; Z -= rhs.Z; W -= rhs.W; return *this; }
    constexpr Float4& Float4::operator*=(const Float4& rhs) { X *= rhs.X; Y *= rhs.Y; Z *= rhs.Z; W *= rhs.W; return *this; }

    //- Binary

    //-- Vector | Scalar
    constexpr Float4 operator+(const Float4& lhs, float32 rhs) { return Float4(lhs.X + rhs, lhs.Y + rhs, lhs.Z + rhs, lhs.W + rhs); }
    constexpr Float4 operator-(const Float4& lhs, float32 rhs) { return Float4(lhs.X - rhs, lhs.Y - rhs, lhs.Z - rhs, lhs.W - rhs); }
    constexpr Float4 operator*(const Float4& lhs, float32 rhs) { return Float4(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs, lhs.W * rhs); }

    //-- Scalar | Vector
    constexpr Float4 operator+(float32 lhs, const Float4& rhs) { return Float4(lhs + rhs.X, lhs + rhs.Y, lhs + rhs.Z, lhs + rhs.W); }
    constexpr Float4 operator-(float32 lhs, const Float4& rhs) { return Float4(lhs - rhs.X, lhs - rhs.Y, lhs - rhs.Z, lhs - rhs.W); }
    constexpr Float4 operator*(float32 lhs, const Float4& rhs) { return Float4(lhs * rhs.X, lhs * rhs.Y, lhs * rhs.Z, lhs * rhs.W); }

    //-- Vector | Vector
    constexpr Float4 operator+(const Float4& lhs, const Float4& rhs) { return Float4(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W + rhs.W); }
    constexpr Float4 operator-(const Float4& lhs, const Float4& rhs) { return Float4(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z, lhs.W - rhs.W); }
    constexpr Float4 operator*(const Float4& lhs, const Float4& rhs) { return Float4(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z, lhs.W * rhs.W); }


    // ========================================================================
    // =============================== Float3x3 ===============================
    // ========================================================================

    constexpr Float3x3::Float3x3(float32 m00, float32 m01, float32 m02,
                                 float32 m10, float32 m11, float32 m12,
                                 float32 m20, float32 m21, float32 m22)
        : C0(m00, m10, m20)
        , C1(m01, m11, m21)
        , C2(m02, m12, m22)
    {
    }

    constexpr Float3x3::Float3x3(const Float3& c0, const Float3& c1, const Float3& c2)
        : C0(c0)
        , C1(c1)
        , C2(c2)
    {
    }

    constexpr Float3x3::Float3x3(const Quaternion& q)
    {
        float32 qxx(q.X * q.X);
        float32 qyy(q.Y * q.Y);
        float32 qzz(q.Z * q.Z);
        float32 qxz(q.X * q.Z);
        float32 qxy(q.X * q.Y);
        float32 qyz(q.Y * q.Z);
        float32 qwx(q.W * q.X);
        float32 qwy(q.W * q.Y);
        float32 qwz(q.W * q.Z);

        float32 m00 = 1.0f - 2.0f * (qyy + qzz);
        float32 m01 = 2.0f * (qxy + qwz);
        float32 m02 = 2.0f * (qxz - qwy);

        float32 m10 = 2.0f * (qxy - qwz);
        float32 m11 = 1.0f - 2.0f * (qxx + qzz);
        float32 m12 = 2.0f * (qyz + qwx);

        float32 m20 = 2.0f * (qxz + qwy);
        float32 m21 = 2.0f * (qyz - qwx);
        float32 m22 = 1.0f - 2.0f * (qxx + qyy);

        C0 = Float3(m00, m10, m20);
        C1 = Float3(m01, m11, m21);
        C2 = Float3(m02, m12, m22);
    }


    //- Compound

    //-- Matrix | Matrix
    constexpr Float3x3& Float3x3::operator+=(const Float3x3& rhs) { C0 += rhs.C0; C1 += rhs.C1; C2 += rhs.C2; return *this; }
    constexpr Float3x3& Float3x3::operator-=(const Float3x3& rhs) { C0 -= rhs.C0; C1 -= rhs.C1; C2 -= rhs.C2; return *this; }
    constexpr Float3x3& Float3x3::operator*=(const Float3x3& rhs) { C0 *= rhs.C0; C1 *= rhs.C1; C2 *= rhs.C2; return *this; }

    //- Binary

    //-- Matrix | Matrix
    constexpr Float3x3 operator+(const Float3x3& lhs, const Float3x3& rhs) { return Float3x3(lhs.C0 + rhs.C0, lhs.C1 + rhs.C1, lhs.C2 + rhs.C2); }
    constexpr Float3x3 operator-(const Float3x3& lhs, const Float3x3& rhs) { return Float3x3(lhs.C0 - rhs.C0, lhs.C1 - rhs.C1, lhs.C2 - rhs.C2); }
    constexpr Float3x3 operator*(const Float3x3& lhs, const Float3x3& rhs) { return Float3x3(lhs.C0 * rhs.C0, lhs.C1 * rhs.C1, lhs.C2 * rhs.C2); }


    // ========================================================================
    // =============================== Float4x4 ===============================
    // ========================================================================

    constexpr Float4x4::Float4x4(float32 m00, float32 m01, float32 m02, float32 m03,
                                 float32 m10, float32 m11, float32 m12, float32 m13,
                                 float32 m20, float32 m21, float32 m22, float32 m23,
                                 float32 m30, float32 m31, float32 m32, float32 m33)
        : C0(m00, m10, m20, m30)
        , C1(m01, m11, m21, m31)
        , C2(m02, m12, m22, m32)
        , C3(m03, m13, m23, m33)
    {
    }

    constexpr Float4x4::Float4x4(const Float4& c0, const Float4& c1, const Float4& c2, const Float4& c3)
        : C0(c0)
        , C1(c1)
        , C2(c2)
        , C3(c3)
    {
    }


    //- Compound

    //-- Matrix | Matrix
    constexpr Float4x4& Float4x4::operator+=(const Float4x4& rhs) { C0 += rhs.C0; C1 += rhs.C1; C2 += rhs.C2; C3 += rhs.C3; return *this; }
    constexpr Float4x4& Float4x4::operator-=(const Float4x4& rhs) { C0 -= rhs.C0; C1 -= rhs.C1; C2 -= rhs.C2; C3 -= rhs.C3; return *this; }
    constexpr Float4x4& Float4x4::operator*=(const Float4x4& rhs) { C0 *= rhs.C0; C1 *= rhs.C1; C2 *= rhs.C2; C3 *= rhs.C3; return *this; }

    //- Binary

    //-- Matrix | Matrix
    constexpr Float4x4 operator+(const Float4x4& lhs, const Float4x4& rhs) { return Float4x4(lhs.C0 + rhs.C0, lhs.C1 + rhs.C1, lhs.C2 + rhs.C2, lhs.C3 + rhs.C3); }
    constexpr Float4x4 operator-(const Float4x4& lhs, const Float4x4& rhs) { return Float4x4(lhs.C0 - rhs.C0, lhs.C1 - rhs.C1, lhs.C2 - rhs.C2, lhs.C3 - rhs.C3); }
    constexpr Float4x4 operator*(const Float4x4& lhs, const Float4x4& rhs) { return Float4x4(lhs.C0 * rhs.C0, lhs.C1 * rhs.C1, lhs.C2 * rhs.C2, lhs.C3 * rhs.C3); }


    constexpr Float4x4 Float4x4::Zero()
    {
        return Float4x4(0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f);
    }

    constexpr Float4x4 Float4x4::Identity()
    {
        return Float4x4(1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);
    }


    constexpr Float4x4 Float4x4::Translate(const Float3& translation)
    {
        return Float4x4(0.0f, 0.0f, 0.0f, translation.X,
                        0.0f, 0.0f, 0.0f, translation.Y,
                        0.0f, 0.0f, 0.0f, translation.Z,
                        0.0f, 0.0f, 0.0f,          1.0f);
    }

    constexpr Float4x4 Float4x4::Scale(float32 scale)
    {
        return Float4x4(scale,  0.0f,  0.0f, 0.0f,
                         0.0f, scale,  0.0f, 0.0f,
                         0.0f,  0.0f, scale, 0.0f,
                         0.0f,  0.0f,  0.0f, 1.0f);
    }

    constexpr Float4x4 Float4x4::Scale(const Float3& scale)
    {
        return Float4x4(scale.X,    0.0f,    0.0f, 0.0f,
                           0.0f, scale.Y,    0.0f, 0.0f,
                           0.0f,    0.0f, scale.Z, 0.0f,
                           0.0f,    0.0f,    0.0f, 1.0f);
    }

    constexpr Float4x4 Float4x4::TRS(const Float3& translation, const Quaternion& rotation, const Float3& scale)
    {
        Float3x3 r(rotation);

        return Float4x4(Float4(r.C0 * scale.X, 0.0f),
                        Float4(r.C1 * scale.Y, 0.0f),
                        Float4(r.C2 * scale.Z, 0.0f),
                        Float4(translation, 1.0f));
    }


    Float4x4 Float4x4::Perspective(float32 verticalFov, float32 aspect, float32 near, float32 far)
    {
        float32 cotangent = 1.0f / std::tan(verticalFov * 0.5f);
        float32 rcp = 1.0f / (near - far);

        float32 m00 = cotangent / aspect;
        float32 m22 = (far + near) * rcp;
        float32 m23 = 2.0f * near * far * rcp;

        return Float4x4( m00,      0.0f,  0.0f, 0.0f,
                        0.0f, cotangent,  0.0f, 0.0f,
                        0.0f,      0.0f,   m22,  m23,
                        0.0f,      0.0f, -1.0f, 0.0f);
    }


    // ========================================================================
    // ============================== Quaternion ==============================
    // ========================================================================

    constexpr Quaternion::Quaternion(float32 x, float32 y, float32 z, float32 w)
        : X(x)
        , Y(y)
        , Z(z)
        , W(w)
    {
    }

    constexpr Quaternion::Quaternion(const Float3& xyz, float32 w)
        : X(xyz.X)
        , Y(xyz.Y)
        , Z(xyz.Z)
        , W(w)
    {
    }

    constexpr Quaternion::Quaternion(const Float4& value)
        : X(value.X)
        , Y(value.Y)
        , Z(value.Z)
        , W(value.W)
    {
    }


    constexpr Quaternion Quaternion::Mul(const Quaternion& lhs, const Quaternion& rhs)
    {
        float32 x = lhs.W * rhs.X + lhs.X * rhs.W + lhs.Y * rhs.Z - lhs.Z * rhs.Y;
        float32 y = lhs.W * rhs.Y + lhs.Y * rhs.W + lhs.Z * rhs.X - lhs.X * rhs.Z;
        float32 z = lhs.W * rhs.Z + lhs.Z * rhs.W + lhs.X * rhs.Y - lhs.Y * rhs.X;
        float32 w = lhs.W * rhs.W - lhs.X * rhs.X - lhs.Y * rhs.Y - lhs.Z * rhs.Z;

        return Quaternion(x, y, z, w);
    }

    constexpr Float3 Quaternion::Mul(const Quaternion& lhs, const Float3& rhs)
    {
        Float3 lhsXYZ(lhs.X, lhs.Y, lhs.Z);
        Float3 t = 2.0f * Math::Cross(lhsXYZ, rhs);

        return rhs + lhs.W * t + Math::Cross(lhsXYZ, t);
    }


    Quaternion Quaternion::AxisAngle(const Float3& axis, float32 angle)
    {
        float32 angleSin, angleCos;
        Math::SinCos(angle * 0.5f, angleSin, angleCos);

        return Quaternion(axis * angleSin, angleCos);
    }

} // export namespace Copium


export namespace Copium
{

    constexpr Float3 Math::Radians(const Float3& degrees) { return degrees * k_DegreeToRadians; }
    constexpr Float4 Math::Radians(const Float4& degrees) { return degrees * k_DegreeToRadians; }

    constexpr Float3 Math::Degrees(const Float3& radians) { return radians * k_RadiansToDegree; }
    constexpr Float4 Math::Degrees(const Float4& radians) { return radians * k_RadiansToDegree; }

    constexpr float32 Math::Dot(const Float3& lhs, const Float3& rhs) { return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z; }
    constexpr float32 Math::Dot(const Float4& lhs, const Float4& rhs) { return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z + lhs.W * rhs.W; }

    constexpr Float3 Math::Cross(const Float3& lhs, const Float3& rhs)
    {
        return Float3(lhs.Y * rhs.Z - lhs.Z * rhs.Y,
                      lhs.Z * rhs.X + lhs.X * rhs.Z,
                      lhs.X * rhs.Y - lhs.Y * rhs.X);
    }

} // export namespace Copium::Math