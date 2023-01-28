export module Hesternal.Core:Literals;


// TODO(v.matushkin): Can't compile cause HeaderTool breaks on operator"" parsing
//   can't wait for when I'm gonna replace custom parser with clang ast analyzer


// import :Constants;
// import :FundamentalTypes;
// 
// 
// // https://akrzemi1.wordpress.com/2011/05/06/compile-time-computations/
// // https://akrzemi1.wordpress.com/2012/08/12/user-defined-literals-part-i/
// 
// 
// export namespace Hesternal
// {
// 
//     consteval int8 operator"" _i8(uint64 value)
//     {
//         if (value <= Int8::Max)
//         {
//             return static_cast<int8>(value);
//         }
//         throw "value is outside of int8 range";
//     }
// 
//     consteval int16 operator"" _i16(uint64 value)
//     {
//         if (value <= Int16::Max)
//         {
//             return static_cast<int16>(value);
//         }
//         throw "value is outside of int16 range";
//     }
// 
//     consteval int32 operator"" _i32(uint64 value)
//     {
//         if (value <= Int32::Max)
//         {
//             return static_cast<int32>(value);
//         }
//         throw "value is outside of int32 range";
//     }
// 
//     consteval int64 operator"" _i64(uint64 value)
//     {
//         if (value <= Int64::Max)
//         {
//             return static_cast<int64>(value);
//         }
//         throw "value is outside of int64 range";
//     }
// 
// 
//     consteval uint8 operator"" _ui8(uint64 value)
//     {
//         if (value <= UInt8::Max)
//         {
//             return static_cast<uint8>(value);
//         }
//         throw "value is outside of uint8 range";
//     }
// 
//     consteval uint16 operator"" _ui16(uint64 value)
//     {
//         if (value <= UInt16::Max)
//         {
//             return static_cast<uint16>(value);
//         }
//         throw "value is outside of uint16 range";
//     }
// 
//     consteval uint32 operator"" _ui32(uint64 value)
//     {
//         if (value <= UInt32::Max)
//         {
//             return static_cast<uint32>(value);
//         }
//         throw "value is outside of uint32 range";
//     }
// 
//     consteval uint64 operator"" _ui64(uint64 value)
//     {
//         return static_cast<uint64>(value);
//     }
// 
// } // export namespace Hesternal
