#pragma once

#include "Hesternal/Core/Defines.hpp"

#if HS_ENABLE_LOGGING

    // HS_WARNING_PUSH
    // HS_WARNING_DISABLE_MSVC(4189) // fmt/format.h(1812,18): warning C4189: 'zero': local variable is initialized but not referenced
        #include <spdlog/common.h>
        #include <spdlog/logger.h>
    // HS_WARNING_POP

    #define LOG_FUNCTION_DEFINITION ;

    #include <cstdlib>
    #include <memory>

#else

    #define LOG_FUNCTION_DEFINITION {}

#endif // HS_ENABLE_LOGGING


namespace Hesternal
{

#if HS_ENABLE_LOGGING
    enum class LogLevel : int // int32
    {
        Trace    = 0,
        Debug    = 1,
        Info     = 2,
        Warn     = 3,
        Error    = 4,
        Critical = 5,
        Off      = 6,
    };
#endif // HS_ENABLE_LOGGING


    class Debug final
    {
    public:
        static void Init()     LOG_FUNCTION_DEFINITION
        static void Shutdown() LOG_FUNCTION_DEFINITION

#if HS_ENABLE_LOGGING
        static inline std::unique_ptr<spdlog::logger> Logger;
#endif // HS_ENABLE_LOGGING
    };

} // namespace Hesternal


#undef LOG_FUNCTION_DEFINITION


#if HS_ENABLE_LOGGING

    #define HS_LOG_LEVEL(lvl, ...) Hesternal::Debug::Logger->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, static_cast<spdlog::level::level_enum>(lvl), __VA_ARGS__)

    #define HS_LOG_TRACE(...)    HS_LOG_LEVEL(Hesternal::LogLevel::Trace,    __VA_ARGS__)
    #define HS_LOG_DEBUG(...)    HS_LOG_LEVEL(Hesternal::LogLevel::Debug,    __VA_ARGS__)
    #define HS_LOG_INFO(...)     HS_LOG_LEVEL(Hesternal::LogLevel::Info,     __VA_ARGS__)
    #define HS_LOG_WARN(...)     HS_LOG_LEVEL(Hesternal::LogLevel::Warn,     __VA_ARGS__)
    #define HS_LOG_ERROR(...)    HS_LOG_LEVEL(Hesternal::LogLevel::Error,    __VA_ARGS__)
    #define HS_LOG_CRITICAL(...) HS_LOG_LEVEL(Hesternal::LogLevel::Critical, __VA_ARGS__)

    #define HS_LOG_LEVEL_COND(lvl, condition, ...) do { if (static_cast<bool>(condition)) { HS_LOG_LEVEL(lvl, __VA_ARGS__); } } while ((void)0,0)

    #define HS_LOG_TRACE_COND(condition, ...)    HS_LOG_LEVEL_COND(Hesternal::LogLevel::Trace,    condition, __VA_ARGS__)
    #define HS_LOG_DEBUG_COND(condition, ...)    HS_LOG_LEVEL_COND(Hesternal::LogLevel::Debug,    condition, __VA_ARGS__)
    #define HS_LOG_INFO_COND(condition, ...)     HS_LOG_LEVEL_COND(Hesternal::LogLevel::Info,     condition, __VA_ARGS__)
    #define HS_LOG_WARN_COND(condition, ...)     HS_LOG_LEVEL_COND(Hesternal::LogLevel::Warn,     condition, __VA_ARGS__)
    #define HS_LOG_ERROR_COND(condition, ...)    HS_LOG_LEVEL_COND(Hesternal::LogLevel::Error,    condition, __VA_ARGS__)
    #define HS_LOG_CRITICAL_COND(condition, ...) HS_LOG_LEVEL_COND(Hesternal::LogLevel::Critical, condition, __VA_ARGS__)

#else

    #define HS_LOG_LEVEL(lvl, ...) do { } while ((void)0,0)

    #define HS_LOG_TRACE(...)    do { } while ((void)0,0)
    #define HS_LOG_DEBUG(...)    do { } while ((void)0,0)
    #define HS_LOG_INFO(...)     do { } while ((void)0,0)
    #define HS_LOG_WARN(...)     do { } while ((void)0,0)
    #define HS_LOG_ERROR(...)    do { } while ((void)0,0)
    #define HS_LOG_CRITICAL(...) do { } while ((void)0,0)

    #define HS_LOG_LEVEL_COND(lvl, condition, ...) do { } while ((void)0,0)

    #define HS_LOG_TRACE_COND(condition, ...)    do { } while ((void)0,0)
    #define HS_LOG_DEBUG_COND(condition, ...)    do { } while ((void)0,0)
    #define HS_LOG_INFO_COND(condition, ...)     do { } while ((void)0,0)
    #define HS_LOG_WARN_COND(condition, ...)     do { } while ((void)0,0)
    #define HS_LOG_ERROR_COND(condition, ...)    do { } while ((void)0,0)
    #define HS_LOG_CRITICAL_COND(condition, ...) do { } while ((void)0,0)

#endif // HS_ENABLE_LOGGING


#if HS_ENABLE_ASSERTS

    // https://artificial-mind.net/blog/2020/09/19/destructuring-assertions

    #define HS_ASSERT(condition)\
        do\
        {\
            if (!static_cast<bool>(condition)) [[unlikely]]\
            {\
                HS_LOG_CRITICAL("Assertion failed with condition: `" #condition "`");\
                HS_DEBUG_BREAK();\
                abort();\
            }\
        } while ((void)0,0)

    #define HS_ASSERT_MSG(condition, message)\
        do\
        {\
            if (!static_cast<bool>(condition)) [[unlikely]]\
            {\
                HS_LOG_CRITICAL("Assertion failed with condition: `" #condition "`\n\tMessage: {}", (message));\
                HS_DEBUG_BREAK();\
                abort();\
            }\
        } while ((void)0,0)

#else

    // #define HS_ASSERT(condition)              do { HS_UNUSED(condition); } while ((void)0,0)
    // #define HS_ASSERT_MSG(condition, message) do { HS_UNUSED(condition); } while ((void)0,0)

    #define HS_ASSERT(condition)              do { } while ((void)0,0)
    #define HS_ASSERT_MSG(condition, message) do { } while ((void)0,0)

#endif // HS_ENABLE_ASSERTS
