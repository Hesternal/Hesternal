module;

#include <combaseapi.h>

module CopiumEngine.Core.Platform;

import CopiumEngine.Core.Misc.Guid;


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
