export module CopiumEngine.ECS.Components:IComponent;

import <concepts>;


export namespace Copium
{

    struct IComponent
    {
    };


    template<typename T>
    concept CComponent = std::derived_from<T, IComponent>;

} // export namespace Copium
