module;

#include "CopiumEngine/Engine/EngineSettings.hpp"

module CopiumEngine.Engine.EngineSettings;


#if _COP_STATIC_VARIABLE_BUG_FIXED == false
namespace Copium
{

    EngineSettings& EngineSettings::Get() noexcept
    {
        static constinit EngineSettings settings;
        return settings;
    }

} // namespace Copium
#endif
