export module CopiumEngine.Math:Quaternion;

import :Float3;
import :Float3x3;
import :Float4;
import :Functions;
import :VectorFunctions;
import CopiumEngine.Core.CoreTypes;


export namespace Copium
{

    struct Quaternion final
    {
        float32 X;
        float32 Y;
        float32 Z;
        float32 W;


        constexpr Quaternion() noexcept = default;
        constexpr Quaternion(const Quaternion& other) noexcept = default;

        constexpr Quaternion(float32 x, float32 y, float32 z, float32 w) noexcept : X(x), Y(y), Z(z), W(w) {}

        constexpr Quaternion(const Float3& xyz, float32 w) noexcept : X(xyz.X),  Y(xyz.Y),  Z(xyz.Z),  W(w)      {}
        constexpr Quaternion(const Float4& xyzw)           noexcept : X(xyzw.X), Y(xyzw.Y), Z(xyzw.Z), W(xyzw.W) {}

        [[nodiscard]] constexpr Float3x3 ToFloat3x3() const noexcept;

        [[nodiscard]] static constexpr Quaternion Identity() noexcept { return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); }

        [[nodiscard]] static constexpr Quaternion Conjugate(const Quaternion& q) noexcept { return Quaternion(-q.X, -q.Y, -q.Z, q.W); }
        // TODO(v.matushkin): Not tested
        [[nodiscard]] static constexpr Quaternion Inverse(const Quaternion& q) noexcept;

        /// <summary>Returns the result of transforming the quaternion b by the quaternion a.</summary>
        [[nodiscard]] static constexpr Quaternion Mul(const Quaternion& lhs, const Quaternion& rhs) noexcept;
        /// <summary>Returns the result of rotating a vector by a unit quaternion.</summary>
        [[nodiscard]] static constexpr Float3 Mul(const Quaternion& lhs, const Float3& rhs) noexcept;
        [[nodiscard]] static constexpr Float3 Mul(const Float3& rhs, const Quaternion& lhs) noexcept;

        /// <param name="axis">The unit axis of rotation.</param>
        /// <param name="angle">The angle of rotation in radians.</param>
        [[nodiscard]] static Quaternion AxisAngle(const Float3& axis, float32 angle) noexcept;

        /// X - pitch, Y - yaw, Z - roll
        [[nodiscard]] static Quaternion EulerZXY(float32 pitch, float32 yaw, float32 roll) noexcept;
    };


    constexpr Float3x3 Quaternion::ToFloat3x3() const noexcept
    {
        float32 qxx(X * X);
        float32 qyy(Y * Y);
        float32 qzz(Z * Z);
        float32 qxz(X * Z);
        float32 qxy(X * Y);
        float32 qyz(Y * Z);
        float32 qwx(W * X);
        float32 qwy(W * Y);
        float32 qwz(W * Z);

        float32 m00 = 1.0f - 2.0f * (qyy + qzz);
        float32 m01 = 2.0f * (qxy - qwz);
        float32 m02 = 2.0f * (qxz + qwy);

        float32 m10 = 2.0f * (qxy + qwz);
        float32 m11 = 1.0f - 2.0f * (qxx + qzz);
        float32 m12 = 2.0f * (qyz - qwx);

        float32 m20 = 2.0f * (qxz - qwy);
        float32 m21 = 2.0f * (qyz + qwx);
        float32 m22 = 1.0f - 2.0f * (qxx + qyy);

        return Float3x3(Float3(m00, m10, m20),
                        Float3(m01, m11, m21),
                        Float3(m02, m12, m22));
    }


    constexpr Quaternion Quaternion::Inverse(const Quaternion& q) noexcept
    {
        Float4 qVector = Float4(q.X, q.Y, q.Z, q.W);
        Float4 qVectorConjugate = Float4(-q.X, -q.Y, -q.Z, q.W);
        float32 qDot = Math::Dot(qVector, qVector);

        return Quaternion(qVectorConjugate * Math::Rcp(qDot));
    }


    constexpr Quaternion Quaternion::Mul(const Quaternion& lhs, const Quaternion& rhs) noexcept
    {
        float32 x = lhs.W * rhs.X + lhs.X * rhs.W + lhs.Y * rhs.Z - lhs.Z * rhs.Y;
        float32 y = lhs.W * rhs.Y + lhs.Y * rhs.W + lhs.Z * rhs.X - lhs.X * rhs.Z;
        float32 z = lhs.W * rhs.Z + lhs.Z * rhs.W + lhs.X * rhs.Y - lhs.Y * rhs.X;
        float32 w = lhs.W * rhs.W - lhs.X * rhs.X - lhs.Y * rhs.Y - lhs.Z * rhs.Z;

        return Quaternion(x, y, z, w);
    }

    constexpr Float3 Quaternion::Mul(const Quaternion& lhs, const Float3& rhs) noexcept
    {
        Float3 lhsXYZ(lhs.X, lhs.Y, lhs.Z);
        Float3 t = 2.0f * Math::Cross(lhsXYZ, rhs);

        return rhs + lhs.W * t + Math::Cross(lhsXYZ, t);
    }

    constexpr Float3 Quaternion::Mul(const Float3& rhs, const Quaternion& lhs) noexcept
    {
        return Quaternion::Mul(Quaternion::Inverse(lhs), rhs);
    }


    Quaternion Quaternion::AxisAngle(const Float3& axis, float32 angle) noexcept
    {
        const auto [sin, cos] = Math::SinCos(0.5f * angle);
        return Quaternion(axis * sin, cos);
    }


    Quaternion Quaternion::EulerZXY(float32 pitch, float32 yaw, float32 roll) noexcept
    {
        const auto [sin, cos] = Math::SinCos(0.5f * Float3(pitch, yaw, roll));

        return Quaternion(sin.X * cos.Y * cos.Z + sin.Y * sin.Z * cos.X,
                          sin.Y * cos.X * cos.Z - sin.X * sin.Z * cos.Y,
                          sin.Z * cos.X * cos.Y - sin.X * sin.Y * cos.Z,
                          cos.X * cos.Y * cos.Z + sin.Y * sin.Z * sin.X);
    }

} // export namespace Copium
