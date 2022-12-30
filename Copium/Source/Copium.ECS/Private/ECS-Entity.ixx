export module Copium.ECS:Entity;

import Copium.Core;


namespace
{
    using namespace Copium;


    inline constexpr uint32 k_InvalidHandle = UInt32::Max;

} // namespace


export namespace Copium
{

    // NOTE(v.matushkin): Make something like entt::null instead of k_InvalidHandle ?
    enum class Entity : uint32 { Invalid = k_InvalidHandle };

} // export namespace Copium
