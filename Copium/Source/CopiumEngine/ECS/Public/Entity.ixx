export module CopiumEngine.ECS.Entity;

import CopiumEngine.Core.CoreTypes;


namespace
{
    using namespace Copium;


    inline constexpr uint32 k_InvalidHandle = 0xffff'ffff;

} // namespace


export namespace Copium
{

    // NOTE(v.matushkin): Make something like entt::null instead of k_InvalidHandle ?
    enum class Entity : uint32 { Invalid = k_InvalidHandle };

} // export namespace Copium
