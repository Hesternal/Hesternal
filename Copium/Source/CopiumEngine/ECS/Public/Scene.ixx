module;

#include <memory>

export module CopiumEngine.ECS.Scene;

import CopiumEngine.ECS.Entity;


export namespace Copium
{

    class Scene final
    {
    public:
        static inline std::shared_ptr<Entity> SponzaRootEntity;
    };

} // export namespace Copium
