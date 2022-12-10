export module Copium.Math:Constants;

import Copium.Core;

import <limits>;
import <numbers>;


// NOTE(v.matushkin): May be it's better to put this constants in Copium.Core ?


export namespace Copium::Math
{

    namespace Int8
    {

        inline constexpr int8 Max = std::numeric_limits<int8>::max();
        inline constexpr int8 Min = std::numeric_limits<int8>::min();

    } // namespace Int8

    namespace Int16
    {

        inline constexpr int16 Max = std::numeric_limits<int16>::max();
        inline constexpr int16 Min = std::numeric_limits<int16>::min();

    } // namespace Int16

    namespace Int32
    {

        inline constexpr int32 Max = std::numeric_limits<int32>::max();
        inline constexpr int32 Min = std::numeric_limits<int32>::min();

    } // namespace Int32

    namespace Int64
    {

        inline constexpr int64 Max = std::numeric_limits<int64>::max();
        inline constexpr int64 Min = std::numeric_limits<int64>::min();

    } // namespace Int64

    namespace UInt8
    {

        inline constexpr uint8 Max = std::numeric_limits<uint8>::max();
        inline constexpr uint8 Min = std::numeric_limits<uint8>::min();

    } // namespace UInt8

    namespace UInt16
    {

        inline constexpr uint16 Max = std::numeric_limits<uint16>::max();
        inline constexpr uint16 Min = std::numeric_limits<uint16>::min();

    } // namespace UInt16

    namespace UInt32
    {

        inline constexpr uint32 Max = std::numeric_limits<uint32>::max();
        inline constexpr uint32 Min = std::numeric_limits<uint32>::min();

    } // namespace UInt32

    namespace UInt64
    {

        inline constexpr uint64 Max = std::numeric_limits<uint64>::max();
        inline constexpr uint64 Min = std::numeric_limits<uint64>::min();

    } // namespace UInt64

    namespace Float32
    {

        inline constexpr float32 Epsilon  = std::numeric_limits<float32>::epsilon();
        inline constexpr float32 Max      = std::numeric_limits<float32>::max();
        inline constexpr float32 Min      = std::numeric_limits<float32>::min();
        inline constexpr float32 Infinity = std::numeric_limits<float32>::infinity();

        inline constexpr float32 PI = std::numbers::pi_v<float32>;

        inline constexpr float32 Deg2Rad = PI / 180.0f;
        inline constexpr float32 Rad2Deg = 180.0f / PI;

    } // namespace Float32

    namespace Float64
    {

        inline constexpr float64 Epsilon  = std::numeric_limits<float64>::epsilon();
        inline constexpr float64 Max      = std::numeric_limits<float64>::max();
        inline constexpr float64 Min      = std::numeric_limits<float64>::min();
        inline constexpr float64 Infinity = std::numeric_limits<float64>::infinity();

        inline constexpr float64 PI = std::numbers::pi_v<float64>;

        inline constexpr float64 Deg2Rad = PI / 180.0;
        inline constexpr float64 Rad2Deg = 180.0 / PI;

    } // namespace Float64

} // export namespace Copium::Math
