module;

#include "Hesternal/Core/Defines.hpp"

#include <Windows.h>

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get import std;
HS_WARNING_PUSH
HS_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
HS_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
module Hesternal.Platform;

import Hesternal.Core;
HS_WARNING_POP


#ifndef HS_PLATFORM_WINDOWS
    #error This file should be compiled only on Windows
#endif


namespace Hesternal
{

    std::wstring Platform::ToWideString(std::string_view str)
    {
        std::wstring wideString;

        const int32 stringLength = static_cast<int32>(str.length());
        const int32 wideStringLength = MultiByteToWideChar(65001 /*CP_UTF8*/, 0, str.data(), stringLength, nullptr, 0);

        if (wideStringLength != 0)
        {
            wideString.resize(wideStringLength);
            MultiByteToWideChar(65001 /*CP_UTF8*/, 0, str.data(), stringLength, wideString.data(), wideStringLength);
        }

        return wideString;
    }

} // namespace Hesternal
