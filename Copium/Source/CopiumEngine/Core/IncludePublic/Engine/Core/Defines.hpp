#pragma once


#if defined(COP_CONFIGURATION_DEBUG)

    #define COP_ENABLE_ASSERTS                  true
    #define COP_ENABLE_LOGGING                  true
    #define COP_ENABLE_GRAPHICS_API_DEBUG       true
    #define COP_ENABLE_GRAPHICS_API_DEBUG_NAMES true

#elif defined(COP_CONFIGURATION_DEVELOPMENT)

    #define COP_ENABLE_ASSERTS                  true
    #define COP_ENABLE_LOGGING                  true
    #define COP_ENABLE_GRAPHICS_API_DEBUG       true
    #define COP_ENABLE_GRAPHICS_API_DEBUG_NAMES true

#elif defined(COP_CONFIGURATION_RELEASE)

    #define COP_ENABLE_ASSERTS                  false
    #define COP_ENABLE_LOGGING                  false
    #define COP_ENABLE_GRAPHICS_API_DEBUG       false
    #define COP_ENABLE_GRAPHICS_API_DEBUG_NAMES false

#else
    #error Unsupported build configuration
#endif


#if defined(COP_COMPILER_MSVC)

    #define COP_DEBUG_BREAK() __debugbreak()
    #define COP_FORCE_INLINE  __forceinline

    #define COP_WARNING_PUSH __pragma( warning(push) )
    #define COP_WARNING_POP  __pragma( warning(pop) )
    #define COP_WARNING_DISABLE_MSVC(warningNumber) __pragma( warning(disable : warningNumber) )

#else
    #error Unsupported compiler
#endif


// https://stackoverflow.com/a/4030983/19262382
#define COP_UNUSED(x) ((void)(true ? 0 : ((x), void(), 0)))


#define _CHT_COMBINE_FILE_PATH_AND_LINE_INNER(filePath, line) filePath##_##line
#define _CHT_COMBINE_FILE_PATH_AND_LINE(filePath, line) _CHT_COMBINE_FILE_PATH_AND_LINE_INNER(filePath, line)

#define CHT_ENUM(...) _CHT_COMBINE_FILE_PATH_AND_LINE(CHT_FILE_PATH, __LINE__);
#define CHT_PROPERTY(...)
#define CHT_STRUCT(...)

#define CHT_GENERATED_BODY() _CHT_COMBINE_FILE_PATH_AND_LINE(CHT_FILE_PATH, __LINE__);
