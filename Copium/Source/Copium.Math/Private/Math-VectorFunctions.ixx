export module Copium.Math:VectorFunctions;

import :Constants;
import :Float2;
import :Float3;
import :Float4;
import :Functions;
import :Int2;

import Copium.Core;

import <utility>;


export namespace Copium::Math
{

    [[nodiscard]] constexpr Float2 ToRadians(Float2 degrees)        noexcept { return degrees * Float32::Deg2Rad; }
    [[nodiscard]] constexpr Float3 ToRadians(const Float3& degrees) noexcept { return degrees * Float32::Deg2Rad; }
    [[nodiscard]] constexpr Float4 ToRadians(const Float4& degrees) noexcept { return degrees * Float32::Deg2Rad; }

    [[nodiscard]] constexpr Float2 ToDegrees(Float2 radians)        noexcept { return radians * Float32::Rad2Deg; }
    [[nodiscard]] constexpr Float3 ToDegrees(const Float3& radians) noexcept { return radians * Float32::Rad2Deg; }
    [[nodiscard]] constexpr Float4 ToDegrees(const Float4& radians) noexcept { return radians * Float32::Rad2Deg; }

    // NOTE(v.matushkin): Seems like compilers can't use SVML intrinsics (https://godbolt.org/z/9fdT7f3Tq)

    [[nodiscard]] inline Float2 Sin(Float2 v)        noexcept { return Float2(Sin(v.X), Sin(v.Y)); }
    [[nodiscard]] inline Float3 Sin(const Float3& v) noexcept { return Float3(Sin(v.X), Sin(v.Y), Sin(v.Z)); }
    [[nodiscard]] inline Float4 Sin(const Float4& v) noexcept { return Float4(Sin(v.X), Sin(v.Y), Sin(v.Z), Sin(v.W)); }

    [[nodiscard]] inline Float2 Cos(Float2 v)        noexcept { return Float2(Cos(v.X), Cos(v.Y)); }
    [[nodiscard]] inline Float3 Cos(const Float3& v) noexcept { return Float3(Cos(v.X), Cos(v.Y), Cos(v.Z)); }
    [[nodiscard]] inline Float4 Cos(const Float4& v) noexcept { return Float4(Cos(v.X), Cos(v.Y), Cos(v.Z), Cos(v.W)); }

    [[nodiscard]] inline std::pair<Float2, Float2> SinCos(Float2 v)        noexcept { return { Sin(v), Cos(v) }; }
    [[nodiscard]] inline std::pair<Float3, Float3> SinCos(const Float3& v) noexcept { return { Sin(v), Cos(v) }; }
    [[nodiscard]] inline std::pair<Float4, Float4> SinCos(const Float4& v) noexcept { return { Sin(v), Cos(v) }; }


    [[nodiscard]] constexpr int32   Dot(Int2 lhs, Int2 rhs)                   noexcept { return lhs.X * rhs.X + lhs.Y * rhs.Y; }
    [[nodiscard]] constexpr float32 Dot(Float2 lhs, Float2 rhs)               noexcept { return lhs.X * rhs.X + lhs.Y * rhs.Y; }
    [[nodiscard]] constexpr float32 Dot(const Float3& lhs, const Float3& rhs) noexcept { return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z; }
    [[nodiscard]] constexpr float32 Dot(const Float4& lhs, const Float4& rhs) noexcept { return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z + lhs.W * rhs.W; }

    [[nodiscard]] constexpr Float3 Cross(const Float3& lhs, const Float3& rhs) noexcept
    {
        return Float3(lhs.Y * rhs.Z - lhs.Z * rhs.Y,
                      lhs.Z * rhs.X - lhs.X * rhs.Z,
                      lhs.X * rhs.Y - lhs.Y * rhs.X);
    }

} // export namespace Copium::Math


// NOTE(v.matushkin): Not gonna work because need to pass T::BaseType to MathTypeArgument<>
//   not sure if I should use templates at all or just CTRL+C CTRL+V
//
// template<typename T>
// using MathTypeArgument = std::conditional<sizeof(T) <= 8, T, const T&>::type;
//
// template<typename T>
// [[nodiscard]] constexpr T ToRadians(MathTypeArgument<T> degrees) noexcept { return degrees * Constants::Deg2Rad; }
