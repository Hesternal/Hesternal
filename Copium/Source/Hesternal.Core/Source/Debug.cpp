#include "Copium/Core/Debug.hpp"

#if COP_ENABLE_LOGGING

#include <spdlog/sinks/stdout_color_sinks.h>

#include <utility>


namespace Hesternal
{

    void Debug::Init()
    {
        auto colorSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        Logger = std::make_unique<spdlog::logger>("Engine", std::move(colorSink));
        Logger->set_level(spdlog::level::trace);
        Logger->set_pattern("[%H:%M:%S.%e][%s:%#] %^%v%$");
    }

    void Debug::Shutdown()
    {
        Logger.reset();
    }

} // namespace Hesternal

#endif // COP_ENABLE_LOGGING
