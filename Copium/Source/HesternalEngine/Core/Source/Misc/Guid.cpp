module HesternalEngine.Core.Misc.Guid;

import HesternalEngine.Core.Platform;


namespace Hesternal
{

    Guid Guid::Generate()
    {
        Guid guid;
        Platform::GenerateGuid(guid);
        return guid;
    }

} // namespace Hesternal
