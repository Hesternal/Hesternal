module;

#include "Hesternal/Core/Defines.hpp"

export module Hesternal.Math:Functions;

import Hesternal.Core;

import <algorithm>;
import <bit>;
import <cmath>;
import <utility>;


export namespace Hesternal::Math
{

    // NOTE(v.matushkin): Questionable function
    [[nodiscard]] HS_FORCE_INLINE constexpr float32 Rcp(float32 x) noexcept { return 1.0f / x; }


    // TODO(v.matushkin): std::abs is constexpr since c++23, but MSVC has not imlpemented it yet
    [[nodiscard]] HS_FORCE_INLINE int32   Abs(int32 x)   noexcept { return std::abs(x); }
    [[nodiscard]] HS_FORCE_INLINE int64   Abs(int64 x)   noexcept { return std::abs(x); }
    [[nodiscard]] HS_FORCE_INLINE float32 Abs(float32 x) noexcept { return std::abs(x); }
    [[nodiscard]] HS_FORCE_INLINE float64 Abs(float64 x) noexcept { return std::abs(x); }

    // TODO(v.matushkin): Make as constexpr after std::abs will be constexpr
    [[nodiscard]] HS_FORCE_INLINE bool IsNearlyEqual(float32 a, float32 b, float32 epsilon = Float32::Epsilon) noexcept
    {
        return Abs(a - b) <= epsilon;
    }
    [[nodiscard]] HS_FORCE_INLINE bool IsNearlyEqual(float64 a, float64 b, float64 epsilon = Float64::Epsilon) noexcept
    {
        return Abs(a - b) <= epsilon;
    }

    [[nodiscard]] HS_FORCE_INLINE bool IsNearlyZero(float32 x, float32 epsilon = Float32::Epsilon) noexcept
    {
        return Abs(x) <= epsilon;
    }
    [[nodiscard]] HS_FORCE_INLINE bool IsNearlyZero(float64 x, float64 epsilon = Float64::Epsilon) noexcept
    {
        return Abs(x) <= epsilon;
    }

    [[nodiscard]] HS_FORCE_INLINE constexpr int8   Max(int8 a, int8 b)     noexcept { return std::max(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr int16  Max(int16 a, int16 b)   noexcept { return std::max(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr int32  Max(int32 a, int32 b)   noexcept { return std::max(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr int64  Max(int64 a, int64 b)   noexcept { return std::max(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr uint8  Max(uint8 a, uint8 b)   noexcept { return std::max(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr uint16 Max(uint16 a, uint16 b) noexcept { return std::max(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr uint32 Max(uint32 a, uint32 b) noexcept { return std::max(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr uint64 Max(uint64 a, uint64 b) noexcept { return std::max(a, b); }

    [[nodiscard]] HS_FORCE_INLINE constexpr int8   Min(int8 a, int8 b)     noexcept { return std::min(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr int16  Min(int16 a, int16 b)   noexcept { return std::min(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr int32  Min(int32 a, int32 b)   noexcept { return std::min(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr int64  Min(int64 a, int64 b)   noexcept { return std::min(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr uint8  Min(uint8 a, uint8 b)   noexcept { return std::min(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr uint16 Min(uint16 a, uint16 b) noexcept { return std::min(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr uint32 Min(uint32 a, uint32 b) noexcept { return std::min(a, b); }
    [[nodiscard]] HS_FORCE_INLINE constexpr uint64 Min(uint64 a, uint64 b) noexcept { return std::min(a, b); }

    [[nodiscard]] HS_FORCE_INLINE float32 Sqrt(float32 x) noexcept { return std::sqrt(x); }
    [[nodiscard]] HS_FORCE_INLINE float64 Sqrt(float64 x) noexcept { return std::sqrt(x); }


    //- Trigonometry

    [[nodiscard]] HS_FORCE_INLINE float32 Sin(float32 x) noexcept { return std::sin(x); }
    [[nodiscard]] HS_FORCE_INLINE float64 Sin(float64 x) noexcept { return std::sin(x); }

    [[nodiscard]] HS_FORCE_INLINE float32 Cos(float32 x) noexcept { return std::cos(x); }
    [[nodiscard]] HS_FORCE_INLINE float64 Cos(float64 x) noexcept { return std::cos(x); }

    [[nodiscard]] inline std::pair<float32, float32> SinCos(float32 x) noexcept { return { Sin(x), Cos(x) }; }
    [[nodiscard]] inline std::pair<float64, float64> SinCos(float64 x) noexcept { return { Sin(x), Cos(x) }; }

    [[nodiscard]] HS_FORCE_INLINE float32 Tan(float32 x) noexcept { return std::tan(x); }
    [[nodiscard]] HS_FORCE_INLINE float64 Tan(float64 x) noexcept { return std::tan(x); }


    [[nodiscard]] HS_FORCE_INLINE constexpr float32 ToRadians(float32 degrees) noexcept { return degrees * Float32::Deg2Rad; }
    [[nodiscard]] HS_FORCE_INLINE constexpr float64 ToRadians(float64 degrees) noexcept { return degrees * Float64::Deg2Rad; }

    [[nodiscard]] HS_FORCE_INLINE constexpr float32 ToDegrees(float32 radians) noexcept { return radians * Float32::Rad2Deg; }
    [[nodiscard]] HS_FORCE_INLINE constexpr float64 ToDegrees(float64 radians) noexcept { return radians * Float64::Rad2Deg; }


    // TODO(v.matushkin): Use "blsr" instruction? https://stackoverflow.com/a/39602456/19262382
    //  MSVC with /O2 and /arch:AVX2 is using it
    [[nodiscard]] HS_FORCE_INLINE constexpr bool IsPowerOfTwo(uint8 x)  noexcept { return std::has_single_bit(x); }
    [[nodiscard]] HS_FORCE_INLINE constexpr bool IsPowerOfTwo(uint16 x) noexcept { return std::has_single_bit(x); }
    [[nodiscard]] HS_FORCE_INLINE constexpr bool IsPowerOfTwo(uint32 x) noexcept { return std::has_single_bit(x); }
    [[nodiscard]] HS_FORCE_INLINE constexpr bool IsPowerOfTwo(uint64 x) noexcept { return std::has_single_bit(x); }

} // export namespace Hesternal::Math


// NOTE(v.matushkin): Not sure about templates
//
// template<typename T> requires CFloatingType<T>
// [[nodiscard]] HS_FORCE_INLINE T Sin(T x) noexcept { return std::sin(x); }
// 
// template float32 Sin<float32>(float32 x);
// template float64 Sin<float64>(float64 x);
// 
// template<typename T> requires CFloatingType<T>
// [[nodiscard]] HS_FORCE_INLINE T Cos(T x) noexcept { return std::cos(x); }
// 
// template float32 Cos<float32>(float32 x);
// template float64 Cos<float64>(float64 x);
// 
// template<typename T> requires CFloatingType<T>
// [[nodiscard]] std::pair<T, T> SinCos(T x) noexcept { return { Sin(x), Cos(x) }; }
// 
// template std::pair<float32, float32> SinCos(float32 x);
// template std::pair<float64, float64> SinCos(float64 x);
// 
// 
// template<typename T> requires CFloatingType<T>
// [[nodiscard]] constexpr T ToRadians(T degrees) noexcept { return degrees * Constants::Deg2Rad; }
// 
// template float32 ToRadians<float32>(float32 degrees);
// template float64 ToRadians<float64>(float64 degrees);
// 
// template<typename T> requires CFloatingType<T>
// [[nodiscard]] constexpr T ToDegrees(T radians) noexcept { return radians * Constants::Rad2Deg; }
// 
// template float32 ToDegrees<float32>(float32 radians);
// template float64 ToDegrees<float64>(float64 radians);
//
//
// template<typename T> requires CUnsignedIntegralType<T>
// [[nodiscard]] constexpr bool IsPowerOfTwo(T value) noexcept { return std::has_single_bit(value); }
// 
// template bool IsPowerOfTwo<uint8>(uint8 value);
// template bool IsPowerOfTwo<uint16>(uint16 value);
// template bool IsPowerOfTwo<uint32>(uint32 value);
// template bool IsPowerOfTwo<uint64>(uint64 value);
