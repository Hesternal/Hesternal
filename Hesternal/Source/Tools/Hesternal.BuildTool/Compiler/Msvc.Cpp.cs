namespace Hesternal.BuildTool.Old;


public static partial class Msvc
{
    #region General

    [CompilerFlag.Enum]
    public enum DebugInformationFormat
    {
        /// <summary>
        /// Produces no debugging information, so compilation may be faster.
        /// </summary>
        None,
        /// <summary>
        /// Produces object files that also contain full symbolic debugging information for use with the debugger.
        /// </summary>
        /// <remarks>
        /// /Z7
        /// </remarks>
        [CompilerFlag.EnumOption("/Z7")]
        C7Compatible,
        /// <summary>
        /// Produces a separate PDB file that contains all the symbolic debugging information for use with the debugger.
        /// </summary>
        /// <remarks>
        /// /Zi
        /// </remarks>
        [CompilerFlag.EnumOption("/Zi")]
        ProgramDatabase,
        /// <summary>
        /// Similar to /Zi, but it produces a PDB file in a format that supports the Edit and Continue feature.
        /// </summary>
        /// <remarks>
        /// /ZI
        /// </remarks>
        [CompilerFlag.EnumOption("/ZI")]
        ProgramDatabaseForEditAndContinue,
    }

    [CompilerFlag.Enum]
    public enum WarningLevel
    {
        /// <remarks>
        /// /W0
        /// </remarks>
        [CompilerFlag.EnumOption("/W0")]
        TurnOffAllWarnings,
        /// <remarks>
        /// /W1
        /// </remarks>
        [CompilerFlag.EnumOption("/W1")]
        Level1,
        /// <remarks>
        /// /W2
        /// </remarks>
        [CompilerFlag.EnumOption("/W2")]
        Level2,
        /// <remarks>
        /// /W3
        /// </remarks>
        [CompilerFlag.EnumOption("/W3")]
        Level3,
        /// <remarks>
        /// /W4
        /// </remarks>
        [CompilerFlag.EnumOption("/W4")]
        Level4,
        /// <remarks>
        /// /Wall
        /// </remarks>
        [CompilerFlag.EnumOption("/Wall")]
        EnableAllWarnings,
    }

    [CompilerFlag.Enum]
    public enum DiagnosticsFormat
    {
        /// <summary>
        /// Provides column information in the diagnostic message, as well as outputting the relevant line of source code with a caret indicating the offending column.
        /// </summary>
        /// <remarks>
        /// /diagnostics:caret
        /// </remarks>
        [CompilerFlag.EnumOption("/diagnostics:caret")]
        Caret,
        /// <summary>
        /// Additionally provides the column number within the line where the diagnostic is issued, where applicable.
        /// </summary>
        /// <remarks>
        /// /diagnostics:column
        /// </remarks>
        [CompilerFlag.EnumOption("/diagnostics:column")]
        Column,
        /// <summary>
        /// Retains the prior, concise diagnostic messages with the line number.
        /// </summary>
        /// <remarks>
        /// /diagnostics:classic
        /// </remarks>
        [CompilerFlag.EnumOption("/diagnostics:classic")]
        Classic,
    }

    #endregion General


    #region Optimization

    [CompilerFlag.Enum]
    public enum Optimization
    {
        Custom,
        /// <remarks>
        /// /Od
        /// </remarks>
        [CompilerFlag.EnumOption("/Od")]
        Disabled,
        /// <summary>
        /// Maximum Optimization (Favor Size). Equivalent to /Og /Os /Oy /Ob2 /Gs /GF /Gy
        /// </summary>
        /// <remarks>
        /// /O1
        /// </remarks>
        [CompilerFlag.EnumOption("/O1")]
        MinSpace,
        /// <summary>
        /// Maximum Optimization (Favor Speed). Equivalent to /Og /Oi /Ot /Oy /Ob2 /Gs /GF /Gy
        /// </summary>
        /// <remarks>
        /// /O2
        /// </remarks>
        [CompilerFlag.EnumOption("/O2")]
        MaxSpeed,
        /// <summary>
        /// Equivalent to /Og /Oi /Ot /Oy /Ob2
        /// </summary>
        /// <remarks>
        /// /Ox
        /// </remarks>
        [CompilerFlag.EnumOption("/Ox")]
        Full,
    }

    [CompilerFlag.Enum]
    public enum InlineFunctionExpansion
    {
        Default,
        /// <summary>
        /// Disables inline expansion, which is on by default.
        /// </summary>
        /// <remarks>
        /// /Ob0
        /// </remarks>
        [CompilerFlag.EnumOption("/Ob0")]
        Disabled,
        /// <summary>
        /// Expands only functions marked as inline, __inline, __forceinline or __inline or, in a C++ member function, defined within a class declaration.
        /// </summary>
        /// <remarks>
        /// /Ob1
        /// </remarks>
        [CompilerFlag.EnumOption("/Ob1")]
        OnlyExplicitInline,
        /// <summary>
        /// Expands functions marked as inline or __inline and any other function that the compiler chooses (expansion occurs at the compiler's discretion, often referred to as auto-inlining).
        /// </summary>
        /// <remarks>
        /// /Ob2
        /// </remarks>
        [CompilerFlag.EnumOption("/Ob2")]
        AnySuitable,
    }

    [CompilerFlag.Enum]
    public enum FavorSizeOrSpeed
    {
        /// <summary>
        /// No size nor speed optimization.
        /// </summary>
        Neither,
        /// <summary>
        /// Favor Small Code. Minimizes the size of EXEs and DLLs by instructing the compiler to favor size over speed.
        /// </summary>
        /// <remarks>
        /// /Os
        /// </remarks>
        [CompilerFlag.EnumOption("/Os")]
        Size, // NOTE(v.matushkin): https://stackoverflow.com/questions/70216761/intel-jcc-erratum-what-is-the-effect-of-prefixes-used-for-mitigation
        /// <summary>
        /// Favor Fast Code. Maximizes the speed of EXEs and DLLs by instructing the compiler to favor speed over size. (This is the default.)
        /// </summary>
        /// <remarks>
        /// /Ot
        /// </remarks>
        [CompilerFlag.EnumOption("/Ot")]
        Speed,
    }

    #endregion Optimization


    #region Code Generation

    [CompilerFlag.Enum]
    public enum ExceptionHandling
    {
        /// <summary>
        /// The exception-handling model that catches asynchronous (structured) and synchronous (C++) exceptions.
        /// </summary>
        /// <remarks>
        /// /EHa
        /// </remarks>
        [CompilerFlag.EnumOption("/EHa")]
        Async,
        /// <summary>
        /// The exception-handling model that catches C++ exceptions only and tells the compiler to assume that extern C functions never throw a C++ exception.
        /// </summary>
        /// <remarks>
        /// /EHsc
        /// </remarks>
        [CompilerFlag.EnumOption("/EHsc")]
        Sync,
        /// <summary>
        /// The exception-handling model that catches C++ exceptions only and tells the compiler to assume that extern C functions do throw an exception.
        /// </summary>
        /// <remarks>
        /// /EHs
        /// </remarks>
        [CompilerFlag.EnumOption("/EHs")]
        SyncCThrow,
        /// <summary>
        /// No exception handling.
        /// </summary>
        False, // NOTE(v.matushkin): Should be lower case false
    }

    [CompilerFlag.Enum]
    public enum BasicRuntimeChecks
    {
        /// <summary>
        /// Enables stack frame run-time error checking.
        /// </summary>
        /// <remarks>
        /// /RTCs
        /// </remarks>
        [CompilerFlag.EnumOption("/RTCs")]
        StackFrameRuntimeCheck,
        /// <summary>
        /// Reports when a variable is used without having been initialized.
        /// </summary>
        /// <remarks>
        /// /RTCu
        /// </remarks>
        [CompilerFlag.EnumOption("/RTCu")]
        UninitializedLocalUsageCheck,
        /// <summary>
        /// Both /RTCs and /RTCu
        /// </summary>
        /// <remarks>
        /// /RTC1 or /RTCsu
        /// </remarks>
        [CompilerFlag.EnumOption("/RTCsu")]
        EnableFastChecks,
        /// <summary>
        /// Default runtime checks.
        /// </summary>
        Default,
    }

    [CompilerFlag.Enum]
    public enum RuntimeLibrary
    {
        /// <summary>
        /// Causes your application to use the multithread, static version of the run-time library.
        /// </summary>
        /// <remarks>
        /// /MT
        /// </remarks>
        [CompilerFlag.EnumOption("/MT")]
        MultiThreaded,
        /// <summary>
        /// "Defines _DEBUG and _MT. This option also causes the compiler to place the library name LIBCMTD.lib into the .obj file so that the linker will use LIBCMTD.lib to resolve external symbols.
        /// </summary>
        /// <remarks>
        /// /MTd
        /// </remarks>
        [CompilerFlag.EnumOption("/MTd")]
        MultiThreadedDebug,
        /// <summary>
        /// Causes your application to use the multithread- and DLL-specific version of the run-time library. Defines _MT and _DLL and causes the compiler to place the library name MSVCRT.lib into the .obj file.
        /// </summary>
        /// <remarks>
        /// /MD
        /// </remarks>
        [CompilerFlag.EnumOption("/MD")]
        MultiThreadedDLL,
        /// <summary>
        /// Defines _DEBUG, _MT, and _DLL and causes your application to use the debug multithread- and DLL-specific version of the run-time library. It also causes the compiler to place the library name MSVCRTD.lib into the .obj file.
        /// </summary>
        /// <remarks>
        /// /MDd
        /// </remarks>
        [CompilerFlag.EnumOption("/MDd")]
        MultiThreadedDebugDLL,
    }

    [CompilerFlag.Enum]
    public enum StructMemberAlignment
    {
        /// <summary>
        /// Packs structures on 1-byte boundaries. Same as /Zp.
        /// </summary>
        /// <remarks>
        /// /Zp1
        /// </remarks>
        [CompilerFlag.EnumOption("/Zp1", "1Byte")]
        _1Byte,
        /// <summary>
        /// Packs structures on 2-byte boundaries.
        /// </summary>
        /// <remarks>
        /// /Zp2
        /// </remarks>
        [CompilerFlag.EnumOption("/Zp2", "2Bytes")]
        _2Bytes,
        /// <summary>
        /// Packs structures on 4-byte boundaries.
        /// </summary>
        /// <remarks>
        /// /Zp4
        /// </remarks>
        [CompilerFlag.EnumOption("/Zp4", "4Bytes")]
        _4Bytes,
        /// <summary>
        /// Packs structures on 8-byte boundaries (default for x86, ARM, and ARM64).
        /// </summary>
        /// <remarks>
        /// /Zp8
        /// </remarks>
        [CompilerFlag.EnumOption("/Zp8", "8Bytes")]
        _8Bytes,
        /// <summary>
        /// Packs structures on 16-byte boundaries (default for x64 and ARM64EC).
        /// </summary>
        /// <remarks>
        /// /Zp16
        /// </remarks>
        [CompilerFlag.EnumOption("/Zp16", "16Bytes")]
        _16Bytes,
        /// <summary>
        /// Default alignment settings.
        /// </summary>
        Default,
    }

    [CompilerFlag.Enum]
    public enum BufferSecurityCheck
    {
        /// <summary>
        /// Disable Security Check.
        /// </summary>
        /// <remarks>
        /// /GS-
        /// </remarks>
        [CompilerFlag.EnumOption("/GS-")]
        False, // NOTE(v.matushkin): Should be lower case false
        /// <summary>
        /// Enable Security Check.
        /// </summary>
        /// <remarks>
        /// /GS
        /// </remarks>
        [CompilerFlag.EnumOption("/GS")]
        True, // NOTE(v.matushkin): Should be lower case true
    }

    [CompilerFlag.Enum]
    public enum ControlFlowGuard
    {
        /// <summary>
        /// Enable Security Check with Guard
        /// </summary>
        /// <remarks>
        /// /guard:cf
        /// </remarks>
        [CompilerFlag.EnumOption("/guard:cf")]
        Guard,
        False, // NOTE(v.matushkin): Should be lower case false
    }

    [CompilerFlag.Enum]
    public enum EnableEnhancedInstructionSet
    {
        /// <remarks>
        /// /arch:SSE
        /// </remarks>
        [CompilerFlag.EnumOption("/arch:SSE")]
        StreamingSIMDExtensions,
        /// <remarks>
        /// /arch:SSE2
        /// </remarks>
        [CompilerFlag.EnumOption("/arch:SSE2")]
        StreamingSIMDExtensions2,
        /// <remarks>
        /// /arch:AVX
        /// </remarks>
        [CompilerFlag.EnumOption("/arch:AVX")]
        AdvancedVectorExtensions,
        /// <remarks>
        /// /arch:AVX2
        /// </remarks>
        [CompilerFlag.EnumOption("/arch:AVX2")]
        AdvancedVectorExtensions2,
        /// <remarks>
        /// /arch:AVX512
        /// </remarks>
        [CompilerFlag.EnumOption("/arch:AVX512")]
        AdvancedVectorExtensions512,
        /// <remarks>
        /// /arch:IA32
        /// </remarks>
        [CompilerFlag.EnumOption("/arch:IA32")]
        NoExtensions,
        NotSet,
    }

    [CompilerFlag.Enum]
    public enum FloatingPointModel
    {
        /// <summary>
        /// Default. Improves the consistency of floating-point tests for equality and inequality.
        /// </summary>
        /// <remarks>
        /// /fp:precise
        /// </remarks>
        [CompilerFlag.EnumOption("/fp:precise")]
        Precise,
        /// <summary>
        /// The strictest floating-point model. Causes fp_contract to be OFF and fenv_access to be ON. /fp:except is implied and can be disabled by explicitly specifying /fp:except-. When used with /fp:except-, enforces strict floating-point semantics but without respect for exceptional events.
        /// </summary>
        /// <remarks>
        /// /fp:strict
        /// </remarks>
        [CompilerFlag.EnumOption("/fp:strict")]
        Strict,
        /// <summary>
        /// Creates the fastest code in the majority of cases.
        /// </summary>
        /// <remarks>
        /// /fp:fast
        /// </remarks>
        [CompilerFlag.EnumOption("/fp:fast")]
        Fast,
    }

    [CompilerFlag.Enum]
    public enum SpectreMitigation
    {
        /// <summary>
        /// Enable Spectre mitigation feature for CVE 2017-5753
        /// </summary>
        /// <remarks>
        /// /Qspectre
        /// </remarks>
        [CompilerFlag.EnumOption("/Qspectre")]
        Spectre,
        /// <summary>
        /// Enable Spectre mitigations for all load instructions
        /// </summary>
        /// <remarks>
        /// /Qspectre-load
        /// </remarks>
        [CompilerFlag.EnumOption("/Qspectre-load")]
        SpectreLoad,
        /// <summary>
        /// Enable Spectre mitigations for all control flow load instructions
        /// </summary>
        /// <remarks>
        /// /Qspectre-load-cf
        /// </remarks>
        [CompilerFlag.EnumOption("/Qspectre-load-cf")]
        SpectreLoadCF,
        /// <summary>
        /// Disabled. Not set.
        /// </summary>
        False, // NOTE(v.matushkin): Should be lower case false
    }

    #endregion Code Generation


    #region Language

    [CompilerFlag.Enum]
    public enum LanguageStandard
    {
        /// <summary>
        /// Default (ISO C++14 Standard)
        /// </summary>
        Default,
        /// <remarks>
        /// /std:c++14
        /// </remarks>
        [CompilerFlag.EnumOption("/std:c++14")]
        stdcpp14,
        /// <remarks>
        /// /std:c++17
        /// </remarks>
        [CompilerFlag.EnumOption("/std:c++17")]
        stdcpp17,
        /// <remarks>
        /// /std:c++20
        /// </remarks>
        [CompilerFlag.EnumOption("/std:c++20")]
        stdcpp20,
        /// <summary>
        /// Preview - Features from the Latest C++ Working Draft
        /// </summary>
        /// <remarks>
        /// /std:c++latest
        /// </remarks>
        [CompilerFlag.EnumOption("/std:c++latest")]
        stdcpplatest,
    }

    [CompilerFlag.Enum]
    public enum LanguageStandard_C
    {
        /// <summary>
        /// Default (Legacy MSVC)
        /// </summary>
        Default,
        /// <summary>
        /// ISO C11 Standard
        /// </summary>
        /// <remarks>
        /// /std:c11
        /// </remarks>
        [CompilerFlag.EnumOption("/std:c11")]
        stdc11,
        /// <summary>
        /// ISO C17 (2018) Standard
        /// </summary>
        /// <remarks>
        /// /std:c17
        /// </remarks>
        [CompilerFlag.EnumOption("/std:c17")]
        stdc17,
    }

    [CompilerFlag.Enum]
    public enum ConformanceMode
    {
        Default,
        /// <remarks>
        /// /permissive-
        /// </remarks>
        [CompilerFlag.EnumOption("/permissive-")]
        True, // NOTE(v.matushkin): Should be lower case true
        /// <remarks>
        /// /permissive
        /// </remarks>
        [CompilerFlag.EnumOption("/permissive")]
        False, // NOTE(v.matushkin): Should be lower case false
    }

    #endregion Language


    #region Precompiled Header

    [CompilerFlag.Enum]
    public enum PrecompiledHeader
    {
        /// <summary>
        /// Instructs the compiler to create a precompiled header (.pch) file that represents the state of compilation at a certain point.
        /// </summary>
        /// <remarks>
        /// /Yc
        /// </remarks>
        [CompilerFlag.EnumOption("/Yc")]
        Create,
        /// <summary>
        /// Instructs the compiler to use an existing precompiled header (.pch) file in the current compilation.
        /// </summary>
        /// <remarks>
        /// /Yu
        /// </remarks>
        [CompilerFlag.EnumOption("/Yu")]
        Use,
        /// <summary>
        /// Not Using Precompiled Headers
        /// </summary>
        NotUsing,
    }

    #endregion Precompiled Header


    #region External Includes

    [CompilerFlag.Enum]
    public enum ExternalWarningLevel
    {
        /// <remarks>
        /// /external:W0
        /// </remarks>
        [CompilerFlag.EnumOption("/external:W0")]
        TurnOffAllWarnings,
        /// <remarks>
        /// /external:W1
        /// </remarks>
        [CompilerFlag.EnumOption("/external:W1")]
        Level1,
        /// <remarks>
        /// /external:W2
        /// </remarks>
        [CompilerFlag.EnumOption("/external:W2")]
        Level2,
        /// <remarks>
        /// /external:W3
        /// </remarks>
        [CompilerFlag.EnumOption("/external:W3")]
        Level3,
        /// <remarks>
        /// /external:W4
        /// </remarks>
        [CompilerFlag.EnumOption("/external:W4")]
        Level4,
        /// <summary>
        /// Inherit Project Warning Level
        /// </summary>
        InheritWarningLevel,
    }

    #endregion External Includes


    #region Advanced

    [CompilerFlag.Enum]
    public enum CallingConvention
    {
        /// <summary>
        /// Specifies the __cdecl calling convention for all functions except C++ member functions and functions marked __stdcall or __fastcall.
        /// </summary>
        /// <remarks>
        /// /Gd
        /// </remarks>
        [CompilerFlag.EnumOption("/Gd")]
        Cdecl,
        /// <summary>
        /// Specifies the __fastcall calling convention for all functions except C++ member sfunctions and functions marked __cdecl or __stdcall. All __fastcall functions must have prototypes.
        /// </summary>
        /// <remarks>
        /// /Gr
        /// </remarks>
        [CompilerFlag.EnumOption("/Gr")]
        FastCall,
        /// <summary>
        /// Specifies the __stdcall calling convention for all functions except C++ member functions and functions marked __cdecl or __fastcall. All __stdcall functions must have prototypes.
        /// </summary>
        /// <remarks>
        /// /Gz
        /// </remarks>
        [CompilerFlag.EnumOption("/Gz")]
        StdCall,
        /// <summary>
        /// Specifies the __vectorcall calling convention for all functions except C++ member functions and functions marked __cdecl, __fastcall, or __stdcall. All __vectorcall functions must have prototypes.
        /// </summary>
        /// <remarks>
        /// /Gv
        /// </remarks>
        [CompilerFlag.EnumOption("/Gv")]
        VectorCall,
    }

    #endregion Advanced
}
