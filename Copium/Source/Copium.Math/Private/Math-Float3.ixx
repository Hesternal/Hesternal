export module Copium.Math:Float3;

import :Float2;
import :Functions;

import Copium.Core;


export namespace Copium
{

    struct Float3 final
    {
        // using BaseType = float32;


        float32 X;
        float32 Y;
        float32 Z;


        constexpr Float3() noexcept = default;
        constexpr Float3(const Float3& other) noexcept = default;

        explicit constexpr Float3(float32 scalar) noexcept : X(scalar), Y(scalar), Z(scalar) {}

        constexpr Float3(float32 x, float32 y, float32 z) noexcept : X(x), Y(y), Z(z) {}

        constexpr Float3(float32 x, Float2 yz) noexcept : X(x),    Y(yz.X), Z(yz.Y) {}
        constexpr Float3(Float2 xy, float32 z) noexcept : X(xy.X), Y(xy.Y), Z(z)    {}

        //- Compound
        //-- Vector | Scalar
        constexpr Float3& operator+=(float32 rhs) noexcept { X += rhs; Y += rhs; Z += rhs; return *this; }
        constexpr Float3& operator-=(float32 rhs) noexcept { X -= rhs; Y -= rhs; Z -= rhs; return *this; }
        constexpr Float3& operator*=(float32 rhs) noexcept { X *= rhs; Y *= rhs; Z *= rhs; return *this; }
        constexpr Float3& operator/=(float32 rhs) noexcept { X /= rhs; Y /= rhs; Z /= rhs; return *this; }
        //-- Vector | Vector
        constexpr Float3& operator+=(const Float3& rhs) noexcept { X += rhs.X; Y += rhs.Y; Z += rhs.Z; return *this; }
        constexpr Float3& operator-=(const Float3& rhs) noexcept { X -= rhs.X; Y -= rhs.Y; Z -= rhs.Z; return *this; }
        constexpr Float3& operator*=(const Float3& rhs) noexcept { X *= rhs.X; Y *= rhs.Y; Z *= rhs.Z; return *this; }
        constexpr Float3& operator/=(const Float3& rhs) noexcept { X /= rhs.X; Y /= rhs.Y; Z /= rhs.Z; return *this; }

        //- Unary
        friend constexpr Float3 operator-(const Float3& rhs) noexcept { return Float3(-rhs.X, -rhs.Y, -rhs.Z); }

        //- Binary
        //-- Vector | Scalar
        friend constexpr Float3 operator+(const Float3& lhs, float32 rhs) noexcept { return Float3(lhs.X + rhs, lhs.Y + rhs, lhs.Z + rhs); }
        friend constexpr Float3 operator-(const Float3& lhs, float32 rhs) noexcept { return Float3(lhs.X - rhs, lhs.Y - rhs, lhs.Z - rhs); }
        friend constexpr Float3 operator*(const Float3& lhs, float32 rhs) noexcept { return Float3(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs); }
        friend constexpr Float3 operator/(const Float3& lhs, float32 rhs) noexcept { return Float3(lhs.X / rhs, lhs.Y / rhs, lhs.Z / rhs); }
        //-- Scalar | Vector
        friend constexpr Float3 operator+(float32 lhs, const Float3& rhs) noexcept { return Float3(lhs + rhs.X, lhs + rhs.Y, lhs + rhs.Z); }
        friend constexpr Float3 operator-(float32 lhs, const Float3& rhs) noexcept { return Float3(lhs - rhs.X, lhs - rhs.Y, lhs - rhs.Z); }
        friend constexpr Float3 operator*(float32 lhs, const Float3& rhs) noexcept { return Float3(lhs * rhs.X, lhs * rhs.Y, lhs * rhs.Z); }
        friend constexpr Float3 operator/(float32 lhs, const Float3& rhs) noexcept { return Float3(lhs / rhs.X, lhs / rhs.Y, lhs / rhs.Z); }
        //-- Vector | Vector
        friend constexpr Float3 operator+(const Float3& lhs, const Float3& rhs) noexcept { return Float3(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z); }
        friend constexpr Float3 operator-(const Float3& lhs, const Float3& rhs) noexcept { return Float3(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z); }
        friend constexpr Float3 operator*(const Float3& lhs, const Float3& rhs) noexcept { return Float3(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z); }
        friend constexpr Float3 operator/(const Float3& lhs, const Float3& rhs) noexcept { return Float3(lhs.X / rhs.X, lhs.Y / rhs.Y, lhs.Z / rhs.Z); }

        [[nodiscard]] inline float32 Length() const noexcept { return Math::Sqrt(X * X + Y * Y + Z * Z); }

        [[nodiscard]] static constexpr Float3 Zero() noexcept { return Float3(0.0f); }
        [[nodiscard]] static constexpr Float3 One() noexcept { return Float3(1.0f); }
    };

} // export namespace Copium
