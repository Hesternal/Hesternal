module;

#include "Copium/Core/Defines.hpp"

export module CopiumEngine.Math:Functions;

import :Constants;

import Copium.Core;

import <bit>;
import <cmath>;
import <utility>;


export namespace Copium::Math
{

    // NOTE(v.matushkin): Questionable function
    [[nodiscard]] COP_FORCE_INLINE constexpr float32 Rcp(float32 x) noexcept { return 1.0f / x; }


    [[nodiscard]] COP_FORCE_INLINE float32 Sqrt(float32 x) noexcept { return std::sqrt(x); }


    [[nodiscard]] COP_FORCE_INLINE float32 Sin(float32 x) noexcept { return std::sin(x); }
    [[nodiscard]] COP_FORCE_INLINE float64 Sin(float64 x) noexcept { return std::sin(x); }

    [[nodiscard]] COP_FORCE_INLINE float32 Cos(float32 x) noexcept { return std::cos(x); }
    [[nodiscard]] COP_FORCE_INLINE float64 Cos(float64 x) noexcept { return std::cos(x); }

    [[nodiscard]] inline std::pair<float32, float32> SinCos(float32 x) noexcept { return { Sin(x), Cos(x) }; }
    [[nodiscard]] inline std::pair<float64, float64> SinCos(float64 x) noexcept { return { Sin(x), Cos(x) }; }

    [[nodiscard]] COP_FORCE_INLINE float32 Tan(float32 x) noexcept { return std::tan(x); }
    [[nodiscard]] COP_FORCE_INLINE float64 Tan(float64 x) noexcept { return std::tan(x); }


    [[nodiscard]] COP_FORCE_INLINE constexpr float32 ToRadians(float32 degrees) noexcept { return degrees * Constants::Deg2Rad; }
    [[nodiscard]] COP_FORCE_INLINE constexpr float64 ToRadians(float64 degrees) noexcept { return degrees * Constants::Deg2Rad; }

    [[nodiscard]] COP_FORCE_INLINE constexpr float32 ToDegrees(float32 radians) noexcept { return radians * Constants::Rad2Deg; }
    [[nodiscard]] COP_FORCE_INLINE constexpr float64 ToDegrees(float64 radians) noexcept { return radians * Constants::Rad2Deg; }


    // TODO(v.matushkin): Use "blsr" instruction? https://stackoverflow.com/a/39602456/19262382
    //  MSVC with /O2 and /arch:AVX2 is using it
    [[nodiscard]] COP_FORCE_INLINE constexpr bool IsPowerOfTwo(uint8 value)  noexcept { return std::has_single_bit(value); }
    [[nodiscard]] COP_FORCE_INLINE constexpr bool IsPowerOfTwo(uint16 value) noexcept { return std::has_single_bit(value); }
    [[nodiscard]] COP_FORCE_INLINE constexpr bool IsPowerOfTwo(uint32 value) noexcept { return std::has_single_bit(value); }
    [[nodiscard]] COP_FORCE_INLINE constexpr bool IsPowerOfTwo(uint64 value) noexcept { return std::has_single_bit(value); }

} // export namespace Copium::Math


// NOTE(v.matushkin): Not sure about templates
//
// template<typename T> requires CFloatingType<T>
// [[nodiscard]] COP_FORCE_INLINE T Sin(T x) noexcept { return std::sin(x); }
// 
// template float32 Sin<float32>(float32 x);
// template float64 Sin<float64>(float64 x);
// 
// template<typename T> requires CFloatingType<T>
// [[nodiscard]] COP_FORCE_INLINE T Cos(T x) noexcept { return std::cos(x); }
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
// template<typename T> requires CUnsignedType<T>
// [[nodiscard]] constexpr bool IsPowerOfTwo(T value) noexcept { return std::has_single_bit(value); }
// 
// template bool IsPowerOfTwo<uint8>(uint8 value);
// template bool IsPowerOfTwo<uint16>(uint16 value);
// template bool IsPowerOfTwo<uint32>(uint32 value);
// template bool IsPowerOfTwo<uint64>(uint64 value);
