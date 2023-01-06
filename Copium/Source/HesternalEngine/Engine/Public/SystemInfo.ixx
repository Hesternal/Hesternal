export module CopiumEngine.Engine.SystemInfo;

import Copium.Core;


export namespace Copium::SystemInfo
{

    // NOTE(v.matushkin): Since I only support Windows/DX11 right now, just hardcode it
    inline constexpr uint32 ConstantBufferAlignment = 256;

} // export namespace Copium::SystemInfo
