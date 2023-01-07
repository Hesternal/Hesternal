export module HesternalEngine.Platform.PlatformTypes;

import Hesternal.Core;

import <string>;


namespace
{
    using namespace Hesternal;


    inline constexpr uint64 k_InvalidHandle = UInt64::Max;

} // namespace


export namespace Hesternal
{

    enum class WindowHandle : uint64 { Invalid = k_InvalidHandle };


    struct WindowDesc
    {
        std::wstring Title;
        uint16       Width;
        uint16       Height;
    };

} // export namespace Hesternal
