module;

#include "Hesternal/Core/Defines.hpp"

#include <combaseapi.h>

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get import std;
HS_WARNING_PUSH
HS_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
HS_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
module Hesternal.Platform;
HS_WARNING_POP


#ifndef HS_PLATFORM_WINDOWS
    #error This file should be compiled only on Windows
#endif


namespace
{
    using namespace Hesternal;


    static_assert(sizeof(Guid) == sizeof(GUID));

} // namespace


namespace Hesternal
{

    Guid Guid::Generate()
    {
        Guid guid;
        CoCreateGuid(reinterpret_cast<GUID*>(&guid));
        return guid;
    }

} // namespace Hesternal
