export module CopiumEngine.Math:Float4;

import :Float2;
import :Float3;
import :Functions;

import Copium.Core;


export namespace Copium
{

    struct Float4 final
    {
        // using BaseType = float32;


        float32 X;
        float32 Y;
        float32 Z;
        float32 W;


        constexpr Float4() noexcept = default;
        constexpr Float4(const Float4& other) noexcept = default;

        explicit constexpr Float4(float32 scalar) noexcept : X(scalar), Y(scalar), Z(scalar), W(scalar) {}

        constexpr Float4(float32 x, float32 y, float32 z, float32 w) noexcept : X(x), Y(y), Z(z), W(w) {}

        constexpr Float4(float32 x, float32 y, Float2 zw) noexcept : X(x),     Y(y),     Z(zw.X),  W(zw.Y)  {}
        constexpr Float4(float32 x, Float2 yz, float32 w) noexcept : X(x),     Y(yz.X),  Z(yz.Y),  W(w)     {}
        constexpr Float4(Float2 xy, float32 z, float32 w) noexcept : X(xy.X),  Y(xy.Y),  Z(z),     W(w)     {}
        constexpr Float4(Float2 xy, Float2 zw)            noexcept : X(xy.X),  Y(xy.Y),  Z(zw.X),  W(zw.Y)  {}

        constexpr Float4(float32 x, const Float3& yzw)    noexcept : X(x),     Y(yzw.X), Z(yzw.Y), W(yzw.Z) {}
        constexpr Float4(const Float3& xyz, float32 w)    noexcept : X(xyz.X), Y(xyz.Y), Z(xyz.Z), W(w)     {}

        //- Compound
        //-- Vector | Scalar
        constexpr Float4& operator+=(float32 rhs) noexcept { X += rhs; Y += rhs; Z += rhs; W += rhs; return *this; }
        constexpr Float4& operator-=(float32 rhs) noexcept { X -= rhs; Y -= rhs; Z -= rhs; W -= rhs; return *this; }
        constexpr Float4& operator*=(float32 rhs) noexcept { X *= rhs; Y *= rhs; Z *= rhs; W *= rhs; return *this; }
        //-- Vector | Vector
        constexpr Float4& operator+=(const Float4& rhs) noexcept { X += rhs.X; Y += rhs.Y; Z += rhs.Z; W += rhs.W; return *this; }
        constexpr Float4& operator-=(const Float4& rhs) noexcept { X -= rhs.X; Y -= rhs.Y; Z -= rhs.Z; W -= rhs.W; return *this; }
        constexpr Float4& operator*=(const Float4& rhs) noexcept { X *= rhs.X; Y *= rhs.Y; Z *= rhs.Z; W *= rhs.W; return *this; }

        //- Unary
        friend constexpr Float4 operator-(const Float4& rhs) noexcept { return Float4(-rhs.X, -rhs.Y, -rhs.Z, -rhs.W); }

        //- Binary
        //-- Vector | Scalar
        friend constexpr Float4 operator+(const Float4& lhs, float32 rhs) noexcept { return Float4(lhs.X + rhs, lhs.Y + rhs, lhs.Z + rhs, lhs.W + rhs); }
        friend constexpr Float4 operator-(const Float4& lhs, float32 rhs) noexcept { return Float4(lhs.X - rhs, lhs.Y - rhs, lhs.Z - rhs, lhs.W - rhs); }
        friend constexpr Float4 operator*(const Float4& lhs, float32 rhs) noexcept { return Float4(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs, lhs.W * rhs); }
        //-- Scalar | Vector
        friend constexpr Float4 operator+(float32 lhs, const Float4& rhs) noexcept { return Float4(lhs + rhs.X, lhs + rhs.Y, lhs + rhs.Z, lhs + rhs.W); }
        friend constexpr Float4 operator-(float32 lhs, const Float4& rhs) noexcept { return Float4(lhs - rhs.X, lhs - rhs.Y, lhs - rhs.Z, lhs - rhs.W); }
        friend constexpr Float4 operator*(float32 lhs, const Float4& rhs) noexcept { return Float4(lhs * rhs.X, lhs * rhs.Y, lhs * rhs.Z, lhs * rhs.W); }
        //-- Vector | Vector
        friend constexpr Float4 operator+(const Float4& lhs, const Float4& rhs) noexcept { return Float4(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W + rhs.W); }
        friend constexpr Float4 operator-(const Float4& lhs, const Float4& rhs) noexcept { return Float4(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z, lhs.W - rhs.W); }
        friend constexpr Float4 operator*(const Float4& lhs, const Float4& rhs) noexcept { return Float4(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z, lhs.W * rhs.W); }

        [[nodiscard]] inline float32 Length() const noexcept { return Math::Sqrt(X * X + Y * Y + Z * Z + W * W); }

        [[nodiscard]] static constexpr Float4 Zero() noexcept { return Float4(0.0f); }
        [[nodiscard]] static constexpr Float4 One() noexcept { return Float4(1.0f); }
    };

} // export namespace Copium
