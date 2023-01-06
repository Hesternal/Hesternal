module CopiumEngine.Core.Misc.Guid;

import CopiumEngine.Core.Platform;


namespace Copium
{

    Guid Guid::Generate()
    {
        Guid guid;
        Platform::GenerateGuid(guid);
        return guid;
    }

} // namespace Copium
