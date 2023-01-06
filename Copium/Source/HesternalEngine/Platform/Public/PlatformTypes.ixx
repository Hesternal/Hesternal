export module CopiumEngine.Platform.PlatformTypes;

import Copium.Core;

import <string>;


namespace
{
    using namespace Copium;


    inline constexpr uint64 k_InvalidHandle = UInt64::Max;

} // namespace


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
