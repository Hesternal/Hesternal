#pragma once


#if defined(HS_CONFIGURATION_DEBUG)

    #define HS_ENABLE_ASSERTS                  true
    #define HS_ENABLE_LOGGING                  true
    #define HS_ENABLE_GRAPHICS_API_DEBUG       true
    #define HS_ENABLE_GRAPHICS_API_DEBUG_NAMES true

#elif defined(HS_CONFIGURATION_DEVELOPMENT)

    #define HS_ENABLE_ASSERTS                  true
    #define HS_ENABLE_LOGGING                  true
    #define HS_ENABLE_GRAPHICS_API_DEBUG       true
    #define HS_ENABLE_GRAPHICS_API_DEBUG_NAMES true

#elif defined(HS_CONFIGURATION_RELEASE)

    #define HS_ENABLE_ASSERTS                  false
    #define HS_ENABLE_LOGGING                  false
    #define HS_ENABLE_GRAPHICS_API_DEBUG       false
    #define HS_ENABLE_GRAPHICS_API_DEBUG_NAMES false

#else
    #error Unsupported build configuration
#endif


#if defined(HS_PLATFORM_WINDOWS)

#else
    #error Unsupported platform
#endif


#if defined(HS_COMPILER_MSVC)

    // NOTE(v.matushkin): Some of MSVC attributes, because there is no fucking documentation on them
    //   https://www.reddit.com/r/programming/comments/g9inos/comment/foyoq62/?utm_source=share&utm_medium=web2x&context=3
    //   - [[msvc::forceinline]] - same as __forceinline on a function definition)
    //   - [[msvc::noinline]] - same as declspec(noinline) on a function
    //   - [[msvc::forceinline_calls]] - goes on statements or statement blocks, and attempts to forceinline all calls in that block
    //   - [[msvc::noinline_calls]] - don't inline any function calls in this block

    #define HS_DEBUG_BREAK() __debugbreak()

    // NOTE(v.matushkin): Need to enable /Ob1 for inline to work in Debug build (but I doubt that I should).
    //   May be I should do what chromium does and in Debug `#define HS_FORCE_INLINE inline`
    #define HS_FORCE_INLINE [[msvc::forceinline]]

    #define HS_WARNING_PUSH __pragma( warning(push) )
    #define HS_WARNING_POP  __pragma( warning(pop) )
    #define HS_WARNING_DISABLE_MSVC(warningNumber) __pragma( warning(disable : warningNumber) )

#elif false && defined(HS_COMPILER_CLANG)

    // NOTE(v.matushkin): __attribute__((always_inline)) for Clang? Seems like Clang attribute works even in Debug?
    //   Documentation says that you should apply this attribute at the call side, not to the function?
    #define HS_FORCE_INLINE [[clang::always_inline]]

#elif false && defined(HS_COMPILER_GCC)

    // NOTE(v.matushkin): Chromium defines this as `inline __attribute__((always_inline))`
    // NOTE(v.matushkin): May be GCC support this attribute like this [[gnu::always_inline]] ?
    #define HS_FORCE_INLINE __attribute__((always_inline))

#else
    #error Unsupported compiler
#endif


// https://stackoverflow.com/a/4030983/19262382
#define HS_UNUSED(x) ((void)(true ? 0 : ((x), void(), 0)))


#define _CHT_COMBINE_FILE_PATH_AND_LINE_INNER(filePath, line) filePath##_##line
#define _CHT_COMBINE_FILE_PATH_AND_LINE(filePath, line) _CHT_COMBINE_FILE_PATH_AND_LINE_INNER(filePath, line)

#define CHT_ENUM(...) _CHT_COMBINE_FILE_PATH_AND_LINE(CHT_FILE_PATH, __LINE__);
#define CHT_PROPERTY(...)
#define CHT_STRUCT(...)
#define CHT_CLASS(...)

#define CHT_GENERATED_BODY() _CHT_COMBINE_FILE_PATH_AND_LINE(CHT_FILE_PATH, __LINE__);


#define _CHT_DECLARE_CONVERTER(Type)\
    friend IBinaryConverter& operator<<(IBinaryConverter& bc, Type& value);
