module;

#include "Copium/Core/Defines.hpp"

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


        [[nodiscard]] static EngineSettings& Get()
        {
            static EngineSettings settings;
            return settings;
        }
    };

} // export namespace Copium
 