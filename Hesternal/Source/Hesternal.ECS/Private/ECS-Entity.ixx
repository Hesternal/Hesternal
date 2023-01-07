export module Hesternal.ECS:Entity;

import Hesternal.Core;


namespace
{
    using namespace Hesternal;


    inline constexpr uint32 k_InvalidHandle = UInt32::Max;

} // namespace


export namespace Hesternal
{

    // NOTE(v.matushkin): Make something like entt::null instead of k_InvalidHandle ?
    enum class Entity : uint32 { Invalid = k_InvalidHandle };

} // export namespace Hesternal
