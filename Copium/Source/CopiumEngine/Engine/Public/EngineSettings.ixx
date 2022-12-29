module;

#include "Copium/Core/Defines.hpp"

#include "CopiumEngine/Engine/EngineSettings.hpp"

export module CopiumEngine.Engine.EngineSettings;

import Copium.Core;


export namespace Copium
{

    enum class GraphicsApi : uint8
    {
// NOTE(v.matushkin): CopiumHeaderTool can't handle prerpocessor yet
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

} // export namespace Copium
 