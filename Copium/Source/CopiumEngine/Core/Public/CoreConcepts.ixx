export module CopiumEngine.Core.CoreConcepts;

import <type_traits>;


export namespace Copium
{

    template<typename T> concept CUnsignedType = std::is_unsigned_v<T>;
    template<typename T> concept CFloatingType = std::is_floating_point_v<T>;

} // export namespace Copium
