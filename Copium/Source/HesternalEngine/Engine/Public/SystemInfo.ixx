export module HesternalEngine.Engine.SystemInfo;

import Hesternal.Core;


export namespace Hesternal::SystemInfo
{

    // NOTE(v.matushkin): Since I only support Windows/DX11 right now, just hardcode it
    inline constexpr uint32 ConstantBufferAlignment = 256;

} // export namespace Hesternal::SystemInfo
