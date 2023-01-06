#pragma once

#include "Copium/Core/Defines.hpp"

#if COP_ENABLE_LOGGING

    // COP_WARNING_PUSH
    // COP_WARNING_DISABLE_MSVC(4189) // fmt/format.h(1812,18): warning C4189: 'zero': local variable is initialized but not referenced
        #include <spdlog/common.h>
        #include <spdlog/logger.h>
    // COP_WARNING_POP

    #define LOG_FUNCTION_DEFINITION ;

    #include <cstdlib>
    #include <memory>

#else

    #define LOG_FUNCTION_DEFINITION {}

#endif // COP_ENABLE_LOGGING


namespace Hesternal
{

#if COP_ENABLE_LOGGING
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
#endif // COP_ENABLE_LOGGING


    class Debug final
    {
    public:
        static void Init()     LOG_FUNCTION_DEFINITION
        static void Shutdown() LOG_FUNCTION_DEFINITION

#if COP_ENABLE_LOGGING
        static inline std::unique_ptr<spdlog::logger> Logger;
#endif // COP_ENABLE_LOGGING
    };

} // namespace Hesternal


#undef LOG_FUNCTION_DEFINITION


#if COP_ENABLE_LOGGING

    #define COP_LOG_LEVEL(lvl, ...) Hesternal::Debug::Logger->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, static_cast<spdlog::level::level_enum>(lvl), __VA_ARGS__)

    #define COP_LOG_TRACE(...)    COP_LOG_LEVEL(Hesternal::LogLevel::Trace,    __VA_ARGS__)
    #define COP_LOG_DEBUG(...)    COP_LOG_LEVEL(Hesternal::LogLevel::Debug,    __VA_ARGS__)
    #define COP_LOG_INFO(...)     COP_LOG_LEVEL(Hesternal::LogLevel::Info,     __VA_ARGS__)
    #define COP_LOG_WARN(...)     COP_LOG_LEVEL(Hesternal::LogLevel::Warn,     __VA_ARGS__)
    #define COP_LOG_ERROR(...)    COP_LOG_LEVEL(Hesternal::LogLevel::Error,    __VA_ARGS__)
    #define COP_LOG_CRITICAL(...) COP_LOG_LEVEL(Hesternal::LogLevel::Critical, __VA_ARGS__)

    #define COP_LOG_LEVEL_COND(lvl, condition, ...) do { if (static_cast<bool>(condition)) { COP_LOG_LEVEL(lvl, __VA_ARGS__); } } while ((void)0,0)

    #define COP_LOG_TRACE_COND(condition, ...)    COP_LOG_LEVEL_COND(Hesternal::LogLevel::Trace,    condition, __VA_ARGS__)
    #define COP_LOG_DEBUG_COND(condition, ...)    COP_LOG_LEVEL_COND(Hesternal::LogLevel::Debug,    condition, __VA_ARGS__)
    #define COP_LOG_INFO_COND(condition, ...)     COP_LOG_LEVEL_COND(Hesternal::LogLevel::Info,     condition, __VA_ARGS__)
    #define COP_LOG_WARN_COND(condition, ...)     COP_LOG_LEVEL_COND(Hesternal::LogLevel::Warn,     condition, __VA_ARGS__)
    #define COP_LOG_ERROR_COND(condition, ...)    COP_LOG_LEVEL_COND(Hesternal::LogLevel::Error,    condition, __VA_ARGS__)
    #define COP_LOG_CRITICAL_COND(condition, ...) COP_LOG_LEVEL_COND(Hesternal::LogLevel::Critical, condition, __VA_ARGS__)

#else

    #define COP_LOG_LEVEL(lvl, ...) do { } while ((void)0,0)

    #define COP_LOG_TRACE(...)    do { } while ((void)0,0)
    #define COP_LOG_DEBUG(...)    do { } while ((void)0,0)
    #define COP_LOG_INFO(...)     do { } while ((void)0,0)
    #define COP_LOG_WARN(...)     do { } while ((void)0,0)
    #define COP_LOG_ERROR(...)    do { } while ((void)0,0)
    #define COP_LOG_CRITICAL(...) do { } while ((void)0,0)

    #define COP_LOG_LEVEL_COND(lvl, condition, ...) do { } while ((void)0,0)

    #define COP_LOG_TRACE_COND(condition, ...)    do { } while ((void)0,0)
    #define COP_LOG_DEBUG_COND(condition, ...)    do { } while ((void)0,0)
    #define COP_LOG_INFO_COND(condition, ...)     do { } while ((void)0,0)
    #define COP_LOG_WARN_COND(condition, ...)     do { } while ((void)0,0)
    #define COP_LOG_ERROR_COND(condition, ...)    do { } while ((void)0,0)
    #define COP_LOG_CRITICAL_COND(condition, ...) do { } while ((void)0,0)

#endif // COP_ENABLE_LOGGING


#if COP_ENABLE_ASSERTS

    // https://artificial-mind.net/blog/2020/09/19/destructuring-assertions

    #define COP_ASSERT(condition)\
        do\
        {\
            if (!static_cast<bool>(condition)) [[unlikely]]\
            {\
                COP_LOG_CRITICAL("Assertion failed with condition: `" #condition "`");\
                COP_DEBUG_BREAK();\
                abort();\
            }\
        } while ((void)0,0)

    #define COP_ASSERT_MSG(condition, message)\
        do\
        {\
            if (!static_cast<bool>(condition)) [[unlikely]]\
            {\
                COP_LOG_CRITICAL("Assertion failed with condition: `" #condition "`\n\tMessage: {}", (message));\
                COP_DEBUG_BREAK();\
                abort();\
            }\
        } while ((void)0,0)

#else

    // #define COP_ASSERT(condition)              do { COP_UNUSED(condition); } while ((void)0,0)
    // #define COP_ASSERT_MSG(condition, message) do { COP_UNUSED(condition); } while ((void)0,0)

    #define COP_ASSERT(condition)              do { } while ((void)0,0)
    #define COP_ASSERT_MSG(condition, message) do { } while ((void)0,0)

#endif // COP_ENABLE_ASSERTS
