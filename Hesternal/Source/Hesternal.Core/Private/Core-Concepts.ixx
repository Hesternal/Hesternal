export module Hesternal.Core:Concepts;

import :FundamentalTypes;

import <concepts>;
import <type_traits>;


namespace
{

    template<typename T, typename... Types>
    inline constexpr bool IsAnyOf = std::disjunction_v<std::is_same<T, Types>...>;

} // namespace


export namespace Hesternal
{

    template<typename T> concept CSignedIntegralType   = IsAnyOf<std::remove_cv_t<T>,  int8,  int16,  int32,  int64>;
    template<typename T> concept CUnsignedIntegralType = IsAnyOf<std::remove_cv_t<T>, uint8, uint16, uint32, uint64>;

    // NOTE(v.matushkin): Differs from the std::is_integral_v, the standard version also includes:
    //   bool, char, wchar_t, char8_t, char16_t, char32_t
    template<typename T> concept CIntegralType = CSignedIntegralType<T> || CUnsignedIntegralType<T>;

    // NOTE(v.matushkin): Some kind of weird typedef/using
    template<typename T> concept CFloatingType = std::floating_point<T>;

    // NOTE(v.matushkin): Since CIntegralType differs from std::is_integral_v, this one is also differs
    //   from std::is_arithmetic_v
    template<typename T> concept CArithmeticType = CIntegralType<T> || CFloatingType<T>;

} // export namespace Hesternal
