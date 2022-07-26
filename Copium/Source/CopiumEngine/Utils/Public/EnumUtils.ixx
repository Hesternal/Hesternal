module;

#include "Engine/Core/CopiumHeaderTool.hpp"

export module CopiumEngine.Utils.EnumUtils;

import <string_view>;


export namespace Copium::EnumUtils
{

    template<typename TEnum> concept CChtEnum = IsChtEnum<TEnum>::value;

    // NOTE(v.matushkin): I can't make enum forward declaration work with modules,
    //  so I can't define template specializations here,
    //  so I can't define this template functions with static_assert,
    //  so there is no clear compilation error when this functions used for not supported enums.
    // NOTE(v.matushkin): Can I consider it fixed? Seems like current scheme with concepts is working,
    //  although MSVC error messages are shit.

    template<CChtEnum TEnum>
    [[nodiscard]] std::string_view ToString(TEnum enumValue);
    // {
    //     // NOTE(v.matushkin): Just use always_false? https://artificial-mind.net/blog/2020/10/03/always-false
    //     static_assert(false && sizeof(TEnum*), "Not supported enum type");
    // }

    template<CChtEnum TEnum>
    [[nodiscard]] TEnum FromString(std::string_view str);
    // {
    //     static_assert(false && sizeof(TEnum*), "Not supported enum type");
    // }

} // export namespace Copium::EnumUtils
