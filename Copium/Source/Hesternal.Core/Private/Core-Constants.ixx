export module Hesternal.Core:Constants;

import :Concepts;
import :FundamentalTypes;

import <limits>;
import <numbers>;


// NOTE(v.matushkin): It's kinda weird that I have FundamentalTypes internal partition
//   and here I have `struct FundamentalType` in a Constants internal partition.


export namespace Hesternal
{

    template<CArithmeticType T>
    struct FundamentalType
    {
        static constexpr T Max = std::numeric_limits<T>::max();
        static constexpr T Min = std::numeric_limits<T>::min();
    };

    template<CFloatingType T>
    struct FloatingType final : FundamentalType<T>
    {
        static constexpr T Epsilon  = std::numeric_limits<T>::epsilon();
        static constexpr T Infinity = std::numeric_limits<T>::infinity();


        static constexpr T PI = std::numbers::pi_v<T>;

        static constexpr T Deg2Rad = PI / static_cast<T>(180.0);
        static constexpr T Rad2Deg = static_cast<T>(180.0) / PI;
    };

    //- Instantiate templates
    template struct FundamentalType<int8>;
    template struct FundamentalType<int16>;
    template struct FundamentalType<int32>;
    template struct FundamentalType<int64>;

    template struct FundamentalType<uint8>;
    template struct FundamentalType<uint16>;
    template struct FundamentalType<uint32>;
    template struct FundamentalType<uint64>;

    template struct FloatingType<float32>;
    template struct FloatingType<float64>;

    //- Typedef
    using Int8  = FundamentalType<int8>;
    using Int16 = FundamentalType<int16>;
    using Int32 = FundamentalType<int32>;
    using Int64 = FundamentalType<int64>;

    using UInt8  = FundamentalType<uint8>;
    using UInt16 = FundamentalType<uint16>;
    using UInt32 = FundamentalType<uint32>;
    using UInt64 = FundamentalType<uint64>;

    using Float32 = FloatingType<float32>;
    using Float64 = FloatingType<float64>;

} // export namespace Hesternal
