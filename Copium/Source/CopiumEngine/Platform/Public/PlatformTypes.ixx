export module CopiumEngine.Platform.PlatformTypes;

import CopiumEngine.Core.CoreTypes;

import <string>;


namespace Copium
{
    inline constexpr uint64 k_InvalidHandle = 0xffff'ffff'ffff'ffff;
}


export namespace Copium
{

    enum class WindowHandle : uint64 { Invalid = k_InvalidHandle };


    struct WindowDesc
    {
        std::wstring Title;
        uint16       Width;
        uint16       Height;
    };

} // export namespace Copium
