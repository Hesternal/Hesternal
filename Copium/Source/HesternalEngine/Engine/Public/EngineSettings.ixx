module;

#include "Hesternal/Core/Defines.hpp"

#include "HesternalEngine/Engine/EngineSettings.hpp"

export module HesternalEngine.Engine.EngineSettings;

import Hesternal.Core;


export namespace Hesternal
{

    enum class GraphicsApi : uint8
    {
// NOTE(v.matushkin): HesternalHeaderTool can't handle prerpocessor yet
// #if COP_PLATFORM_WINDOWS
        DirectX11,
// #endif
    };


    struct EngineSettings
    {
        // TODO(v.matushkin): Remove, temporary
        uint16      RenderWidth;
        uint16      RenderHeight;

        GraphicsApi GraphicsApi;


        [[nodiscard]] static EngineSettings& Get() noexcept
#if _COP_STATIC_VARIABLE_BUG_FIXED == false
            ;
#else
        {
            static EngineSettings settings;
            return settings;
        }
#endif
    };

} // export namespace Hesternal
 