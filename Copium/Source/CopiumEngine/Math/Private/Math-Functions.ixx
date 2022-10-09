export module CopiumEngine.Math:Functions;

import :Constants;
import CopiumEngine.Core.CoreTypes;

import <bit>;
import <cmath>;
import <type_traits>;
import <utility>;


export namespace Copium::Math
{

    [[nodiscard]] constexpr float32 Rcp(float32 x) noexcept { return 1.0f / x; }

    [[nodiscard]] inline std::pair<float32, float32> SinCos(float32 x) noexcept { return { std::sinf(x), std::cosf(x) }; }
    [[nodiscard]] inline std::pair<float64, float64> SinCos(float64 x) noexcept { return { std::sin(x), std::cos(x) }; }

    [[nodiscard]] constexpr float32 Radians(float32 degrees) noexcept { return degrees * k_DegreeToRadians; }
    [[nodiscard]] constexpr float32 Degrees(float32 radians) noexcept { return radians * k_RadiansToDegree; }


    // TODO(v.matushkin): use "blsr" instruction? https://stackoverflow.com/a/39602456/19262382
    //  MSVC with /O2 and /arch:AVX2 is using it
    template<typename T> requires std::is_unsigned_v<T>
    [[nodiscard]] constexpr bool IsPowerOfTwo(T value) noexcept { return std::has_single_bit(value); }

    template bool IsPowerOfTwo<uint8>(uint8 value);
    template bool IsPowerOfTwo<uint16>(uint16 value);
    template bool IsPowerOfTwo<uint32>(uint32 value);
    template bool IsPowerOfTwo<uint64>(uint64 value);

} // export namespace Copium::Math
