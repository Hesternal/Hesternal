module;

#include "HesternalEngine/Engine/EngineSettings.hpp"

module HesternalEngine.Engine.EngineSettings;


#if _COP_STATIC_VARIABLE_BUG_FIXED == false
namespace Hesternal
{

    EngineSettings& EngineSettings::Get() noexcept
    {
        static constinit EngineSettings settings;
        return settings;
    }

} // namespace Hesternal
#endif
