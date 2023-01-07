export module Hesternal.ECS:IComponent;

import <concepts>;


export namespace Hesternal
{

    struct IComponent
    {
    };


    template<typename T>
    concept CComponent = std::derived_from<T, IComponent>;

} // export namespace Hesternal
