module;

#include "Hesternal/Core/Defines.hpp"

#include <combaseapi.h>
#include <Windows.h>

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get import std;
COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
COP_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
module HesternalEngine.Core.Platform;

import Hesternal.Core;

import HesternalEngine.Core.Misc.Guid;
COP_WARNING_POP


#ifndef COP_PLATFORM_WINDOWS
    #error This file should be compiled only on Windows
#endif


namespace Hesternal
{

    void Platform::GenerateGuid(Guid& guid)
    {
        CoCreateGuid(reinterpret_cast<GUID*>(&guid));
    }

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
