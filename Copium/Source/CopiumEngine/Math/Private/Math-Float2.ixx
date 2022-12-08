export module CopiumEngine.Math:Float2;

import :Functions;

import Copium.Core;


export namespace Copium
{

    struct Float2 final
    {
        // using BaseType = float32;


        float32 X;
        float32 Y;


        constexpr Float2() noexcept = default;
        constexpr Float2(const Float2& other) noexcept = default;

        explicit constexpr Float2(float32 scalar) noexcept : X(scalar), Y(scalar) {}

        constexpr Float2(float32 x, float32 y) noexcept : X(x), Y(y) {}

        //- Compound
        //-- Vector | Scalar
        constexpr Float2& operator+=(float32 rhs) noexcept { X += rhs; Y += rhs; return *this; }
        constexpr Float2& operator-=(float32 rhs) noexcept { X -= rhs; Y -= rhs; return *this; }
        constexpr Float2& operator*=(float32 rhs) noexcept { X *= rhs; Y *= rhs; return *this; }
        //-- Vector | Vector
        constexpr Float2& operator+=(Float2 rhs) noexcept { X += rhs.X; Y += rhs.Y; return *this; }
        constexpr Float2& operator-=(Float2 rhs) noexcept { X -= rhs.X; Y -= rhs.Y; return *this; }
        constexpr Float2& operator*=(Float2 rhs) noexcept { X *= rhs.X; Y *= rhs.Y; return *this; }

        //- Unary
        friend constexpr Float2 operator-(Float2 rhs) noexcept { return Float2(-rhs.X, -rhs.Y); }

        //- Binary
        //-- Vector | Scalar
        friend constexpr Float2 operator+(Float2 lhs, float32 rhs) noexcept { return Float2(lhs.X + rhs, lhs.Y + rhs); }
        friend constexpr Float2 operator-(Float2 lhs, float32 rhs) noexcept { return Float2(lhs.X - rhs, lhs.Y - rhs); }
        friend constexpr Float2 operator*(Float2 lhs, float32 rhs) noexcept { return Float2(lhs.X * rhs, lhs.Y * rhs); }
        //-- Scalar | Vector
        friend constexpr Float2 operator+(float32 lhs, Float2 rhs) noexcept { return Float2(lhs + rhs.X, lhs + rhs.Y); }
        friend constexpr Float2 operator-(float32 lhs, Float2 rhs) noexcept { return Float2(lhs - rhs.X, lhs - rhs.Y); }
        friend constexpr Float2 operator*(float32 lhs, Float2 rhs) noexcept { return Float2(lhs * rhs.X, lhs * rhs.Y); }
        //-- Vector | Vector
        friend constexpr Float2 operator+(Float2 lhs, Float2 rhs) noexcept { return Float2(lhs.X + rhs.X, lhs.Y + rhs.Y); }
        friend constexpr Float2 operator-(Float2 lhs, Float2 rhs) noexcept { return Float2(lhs.X - rhs.X, lhs.Y - rhs.Y); }
        friend constexpr Float2 operator*(Float2 lhs, Float2 rhs) noexcept { return Float2(lhs.X * rhs.X, lhs.Y * rhs.Y); }

        [[nodiscard]] inline float32 Length() const noexcept { return Math::Sqrt(X * X + Y * Y); }

        [[nodiscard]] static constexpr Float2 Zero() noexcept { return Float2(0.0f); }
        [[nodiscard]] static constexpr Float2 One() noexcept { return Float2(1.0f); }
    };

} // export namespace Copium
