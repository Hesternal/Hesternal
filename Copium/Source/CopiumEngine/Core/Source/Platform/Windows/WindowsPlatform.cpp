module;

#include "Engine/Core/Defines.hpp"

#include <combaseapi.h>

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get import std;
COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
COP_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
module CopiumEngine.Core.Platform;

import CopiumEngine.Core.Misc.Guid;
COP_WARNING_POP


#ifndef COP_PLATFORM_WINDOWS
    #error This file should be compiled only on Windows
#endif


namespace Copium
{

    void Platform::GenerateGuid(Guid& guid)
    {
        CoCreateGuid(reinterpret_cast<GUID*>(&guid));
    }

} // namespace Copium
