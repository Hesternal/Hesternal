module;

#include "Engine/Core/Defines.hpp"

#include <string>

export module CopiumEngine.Engine.EngineSettings;

import CopiumEngine.Core.CoreTypes;


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
        GraphicsApi  GraphicsApi;

        std::wstring WindowTitle;
        uint16       WindowWidth;
        uint16       WindowHeight;
    };

} // export namespace Copium
