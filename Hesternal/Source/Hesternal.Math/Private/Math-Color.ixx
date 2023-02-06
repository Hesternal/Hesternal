module;

#include "Hesternal/Core/Defines.hpp"

export module Hesternal.Math:Color;

import :Float3;
import :Float4;

import Hesternal.Core;


export namespace Hesternal
{

    // NOTE(v.matushkin): I'm sure at some point Color<uint8> and Color<float16> will be needed.
    //   May be it would be also good to have Color type without alpha, but not sure if it worth.

    // TODO(v.matushkin):
    // - HSV
    // - gamma/linear


    struct Color final
    {
        float32 R;
        float32 G;
        float32 B;
        float32 A;


        constexpr Color() noexcept = default;
        constexpr Color(const Color& other) noexcept = default;

        constexpr Color(float32 r, float32 g, float32 b, float32 a = 1.0f) noexcept : R(r), G(g), B(b), A(a) {}

        explicit constexpr Color(const Float3& rgb)  noexcept : R(rgb.X),  G(rgb.Y),  B(rgb.Z),  A(1.0f) {}
        explicit constexpr Color(const Float4& rgba) noexcept : R(rgba.X), G(rgba.Y), B(rgba.Z), A(rgba.W) {}

        //- Conversion
        explicit constexpr operator Float3() const noexcept { return Float3(R, G, B); }
        explicit constexpr operator Float4() const noexcept { return Float4(R, G, B, A); }

        //- Compound
        //-- Color | Scalar
        constexpr Color& operator+=(float32 rhs) noexcept { R += rhs; G += rhs; B += rhs; A += rhs; return *this; }
        constexpr Color& operator-=(float32 rhs) noexcept { R -= rhs; G -= rhs; B -= rhs; A -= rhs; return *this; }
        constexpr Color& operator*=(float32 rhs) noexcept { R *= rhs; G *= rhs; B *= rhs; A *= rhs; return *this; }
        constexpr Color& operator/=(float32 rhs) noexcept { R /= rhs; G /= rhs; B /= rhs; A /= rhs; return *this; }
        //-- Color | Color
        constexpr Color& operator+=(const Color& rhs) noexcept { R += rhs.R; G += rhs.G; B += rhs.B; A += rhs.A; return *this; }
        constexpr Color& operator-=(const Color& rhs) noexcept { R -= rhs.R; G -= rhs.G; B -= rhs.B; A -= rhs.A; return *this; }
        constexpr Color& operator*=(const Color& rhs) noexcept { R *= rhs.R; G *= rhs.G; B *= rhs.B; A *= rhs.A; return *this; }
        constexpr Color& operator/=(const Color& rhs) noexcept { R /= rhs.R; G /= rhs.G; B /= rhs.B; A /= rhs.A; return *this; }

        //- Binary
        //-- Color | Scalar
        friend constexpr Color operator+(const Color& lhs, float32 rhs) noexcept { return Color(lhs.R + rhs, lhs.G + rhs, lhs.B + rhs, lhs.A + rhs); }
        friend constexpr Color operator-(const Color& lhs, float32 rhs) noexcept { return Color(lhs.R - rhs, lhs.G - rhs, lhs.B - rhs, lhs.A - rhs); }
        friend constexpr Color operator*(const Color& lhs, float32 rhs) noexcept { return Color(lhs.R * rhs, lhs.G * rhs, lhs.B * rhs, lhs.A * rhs); }
        friend constexpr Color operator/(const Color& lhs, float32 rhs) noexcept { return Color(lhs.R / rhs, lhs.G / rhs, lhs.B / rhs, lhs.A / rhs); }
        //-- Scalar | Color
        friend constexpr Color operator+(float32 lhs, const Color& rhs) noexcept { return Color(lhs + rhs.R, lhs + rhs.G, lhs + rhs.B, lhs + rhs.A); }
        friend constexpr Color operator-(float32 lhs, const Color& rhs) noexcept { return Color(lhs - rhs.R, lhs - rhs.G, lhs - rhs.B, lhs - rhs.A); }
        friend constexpr Color operator*(float32 lhs, const Color& rhs) noexcept { return Color(lhs * rhs.R, lhs * rhs.G, lhs * rhs.B, lhs * rhs.A); }
        friend constexpr Color operator/(float32 lhs, const Color& rhs) noexcept { return Color(lhs / rhs.R, lhs / rhs.G, lhs / rhs.B, lhs / rhs.A); }
        //-- Color | Color
        friend constexpr Color operator+(const Color& lhs, const Color& rhs) noexcept { return Color(lhs.R + rhs.R, lhs.G + rhs.G, lhs.B + rhs.B, lhs.A + rhs.A); }
        friend constexpr Color operator-(const Color& lhs, const Color& rhs) noexcept { return Color(lhs.R - rhs.R, lhs.G - rhs.G, lhs.B - rhs.B, lhs.A - rhs.A); }
        friend constexpr Color operator*(const Color& lhs, const Color& rhs) noexcept { return Color(lhs.R * rhs.R, lhs.G * rhs.G, lhs.B * rhs.B, lhs.A * rhs.A); }
        friend constexpr Color operator/(const Color& lhs, const Color& rhs) noexcept { return Color(lhs.R / rhs.R, lhs.G / rhs.G, lhs.B / rhs.B, lhs.A / rhs.A); }


        // NOTE(v.matuhskin): Function or constant?
        // NOTE(v.matushkin): Surely there is nothing bad in making it consteval?
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Black() noexcept   { return Color(  0.0f,   0.0f,   0.0f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Blue() noexcept    { return Color(  0.0f,   0.0f,   1.0f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Brown() noexcept   { return Color(0.647f, 0.164f, 0.164f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Cyan() noexcept    { return Color(  0.0f,   1.0f,   1.0f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Gray() noexcept    { return Color(  0.5f,   0.5f,   0.5f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Green() noexcept   { return Color(  0.0f,   1.0f,   0.0f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Magenta() noexcept { return Color(  1.0f,   0.0f,   1.0f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Orange() noexcept  { return Color(  1.0f, 0.647f,   0.0f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Pink() noexcept    { return Color(  1.0f, 0.752f, 0.796f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Purple() noexcept  { return Color(0.501f,   0.0f, 0.501f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Red() noexcept     { return Color(  1.0f,   0.0f,   0.0f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Silver() noexcept  { return Color(0.752f, 0.752f, 0.752f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Teal() noexcept    { return Color(  0.0f, 0.501f, 0.501f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Violet() noexcept  { return Color(0.933f, 0.509f, 0.933f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color White() noexcept   { return Color(  1.0f,   1.0f,   1.0f); }
        [[nodiscard]] HS_FORCE_INLINE static consteval Color Yellow() noexcept  { return Color(  1.0f,   1.0f,   0.0f); }
    };

} // export namespace Hesternal
