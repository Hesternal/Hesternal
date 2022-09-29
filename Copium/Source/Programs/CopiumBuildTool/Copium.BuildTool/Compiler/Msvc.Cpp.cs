namespace Copium.BuildTool;


public static partial class Msvc
{
    #region General

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
        C7Compatible,
        /// <summary>
        /// Produces a separate PDB file that contains all the symbolic debugging information for use with the debugger.
        /// </summary>
        /// <remarks>
        /// /Zi
        /// </remarks>
        ProgramDatabase,
        /// <summary>
        /// Similar to /Zi, but it produces a PDB file in a format that supports the Edit and Continue feature.
        /// </summary>
        /// <remarks>
        /// /ZI
        /// </remarks>
        ProgramDatabaseForEditAndContinue,
    }

    public enum WarningLevel
    {
        /// <remarks>
        /// /W0
        /// </remarks>
        TurnOffAllWarnings,
        /// <remarks>
        /// /W1
        /// </remarks>
        Level1,
        /// <remarks>
        /// /W2
        /// </remarks>
        Level2,
        /// <remarks>
        /// /W3
        /// </remarks>
        Level3,
        /// <remarks>
        /// /W4
        /// </remarks>
        Level4,
        /// <remarks>
        /// /Wall
        /// </remarks>
        EnableAllWarnings,
    }

    public enum DiagnosticsFormat
    {
        /// <summary>
        /// Provides column information in the diagnostic message, as well as outputting the relevant line of source code with a caret indicating the offending column.
        /// </summary>
        /// <remarks>
        /// /diagnostics:caret
        /// </remarks>
        Caret,
        /// <summary>
        /// Additionally provides the column number within the line where the diagnostic is issued, where applicable.
        /// </summary>
        /// <remarks>
        /// /diagnostics:column
        /// </remarks>
        Column,
        /// <summary>
        /// Retains the prior, concise diagnostic messages with the line number.
        /// </summary>
        /// <remarks>
        /// /diagnostics:classic
        /// </remarks>
        Classic,
    }

    #endregion General


    #region Optimization

    public enum Optimization
    {
        Custom,
        /// <remarks>
        /// /Od
        /// </remarks>
        Disabled,
        /// <summary>
        /// Maximum Optimization (Favor Size). Equivalent to /Og /Os /Oy /Ob2 /Gs /GF /Gy
        /// </summary>
        /// <remarks>
        /// /O1
        /// </remarks>
        MinSpace,
        /// <summary>
        /// Maximum Optimization (Favor Speed). Equivalent to /Og /Oi /Ot /Oy /Ob2 /Gs /GF /Gy
        /// </summary>
        /// <remarks>
        /// /O2
        /// </remarks>
        MaxSpeed,
        /// <summary>
        /// Equivalent to /Og /Oi /Ot /Oy /Ob2
        /// </summary>
        /// <remarks>
        /// /Ox
        /// </remarks>
        Full,
    }

    public enum InlineFunctionExpansion
    {
        Default,
        /// <summary>
        /// Disables inline expansion, which is on by default.
        /// </summary>
        /// <remarks>
        /// /Ob0
        /// </remarks>
        Disabled,
        /// <summary>
        /// Expands only functions marked as inline, __inline, __forceinline or __inline or, in a C++ member function, defined within a class declaration.
        /// </summary>
        /// <remarks>
        /// /Ob1
        /// </remarks>
        OnlyExplicitInline,
        /// <summary>
        /// Expands functions marked as inline or __inline and any other function that the compiler chooses (expansion occurs at the compiler's discretion, often referred to as auto-inlining).
        /// </summary>
        /// <remarks>
        /// /Ob2
        /// </remarks>
        AnySuitable,
    }

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
        Size, // NOTE(v.matushkin): https://stackoverflow.com/questions/70216761/intel-jcc-erratum-what-is-the-effect-of-prefixes-used-for-mitigation
        /// <summary>
        /// Favor Fast Code. Maximizes the speed of EXEs and DLLs by instructing the compiler to favor speed over size. (This is the default.)
        /// </summary>
        /// <remarks>
        /// /Ot
        /// </remarks>
        Speed,
    }

    #endregion Optimization


    #region Code Generation

    public enum ExceptionHandling
    {
        /// <summary>
        /// The exception-handling model that catches asynchronous (structured) and synchronous (C++) exceptions.
        /// </summary>
        /// <remarks>
        /// /EHa
        /// </remarks>
        Async,
        /// <summary>
        /// The exception-handling model that catches C++ exceptions only and tells the compiler to assume that extern C functions never throw a C++ exception.
        /// </summary>
        /// <remarks>
        /// /EHsc
        /// </remarks>
        Sync,
        /// <summary>
        /// The exception-handling model that catches C++ exceptions only and tells the compiler to assume that extern C functions do throw an exception.
        /// </summary>
        /// <remarks>
        /// /EHs
        /// </remarks>
        SyncCThrow,
        /// <summary>
        /// No exception handling.
        /// </summary>
        False, // NOTE(v.matushkin): Should be lower case false
    }

    public enum BasicRuntimeChecks
    {
        /// <summary>
        /// Enables stack frame run-time error checking.
        /// </summary>
        /// <remarks>
        /// /RTCs
        /// </remarks>
        StackFrameRuntimeCheck,
        /// <summary>
        /// Reports when a variable is used without having been initialized.
        /// </summary>
        /// <remarks>
        /// /RTCu
        /// </remarks>
        UninitializedLocalUsageCheck,
        /// <summary>
        /// Both /RTCs and /RTCu
        /// </summary>
        /// <remarks>
        /// /RTC1 or /RTCsu
        /// </remarks>
        EnableFastChecks,
        /// <summary>
        /// Default runtime checks.
        /// </summary>
        Default,
    }

    public enum RuntimeLibrary
    {
        /// <summary>
        /// Causes your application to use the multithread, static version of the run-time library.
        /// </summary>
        /// <remarks>
        /// /MT
        /// </remarks>
        MultiThreaded,
        /// <summary>
        /// "Defines _DEBUG and _MT. This option also causes the compiler to place the library name LIBCMTD.lib into the .obj file so that the linker will use LIBCMTD.lib to resolve external symbols.
        /// </summary>
        /// <remarks>
        /// /MTd
        /// </remarks>
        MultiThreadedDebug,
        /// <summary>
        /// Causes your application to use the multithread- and DLL-specific version of the run-time library. Defines _MT and _DLL and causes the compiler to place the library name MSVCRT.lib into the .obj file.
        /// </summary>
        /// <remarks>
        /// /MD
        /// </remarks>
        MultiThreadedDLL,
        /// <summary>
        /// Defines _DEBUG, _MT, and _DLL and causes your application to use the debug multithread- and DLL-specific version of the run-time library. It also causes the compiler to place the library name MSVCRTD.lib into the .obj file.
        /// </summary>
        /// <remarks>
        /// /MDd
        /// </remarks>
        MultiThreadedDebugDLL,
    }

    // TODO(v.matushkin): Need custom attributes for source generator to support this enum values
    public enum StructMemberAlignment
    {
        /// <summary>
        /// Packs structures on 1-byte boundaries. Same as /Zp.
        /// </summary>
        /// <remarks>
        /// /Zp1
        /// </remarks>
        // 1Byte,
        /// <summary>
        /// Packs structures on 2-byte boundaries.
        /// </summary>
        /// <remarks>
        /// /Zp2
        /// </remarks>
        // 2Bytes,
        /// <summary>
        /// Packs structures on 4-byte boundaries.
        /// </summary>
        /// <remarks>
        /// /Zp4
        /// </remarks>
        // 4Bytes,
        /// <summary>
        /// Packs structures on 8-byte boundaries (default for x86, ARM, and ARM64).
        /// </summary>
        /// <remarks>
        /// /Zp8
        /// </remarks>
        // 8Bytes,
        /// <summary>
        /// Packs structures on 16-byte boundaries (default for x64 and ARM64EC).
        /// </summary>
        /// <remarks>
        /// /Zp16
        /// </remarks>
        // 16Bytes,
        /// <summary>
        /// Default alignment settings.
        /// </summary>
        Default,
    }

    public enum BufferSecurityCheck
    {
        /// <summary>
        /// Disable Security Check.
        /// </summary>
        /// <remarks>
        /// /GS-
        /// </remarks>
        False, // NOTE(v.matushkin): Should be lower case false
        /// <summary>
        /// Enable Security Check.
        /// </summary>
        /// <remarks>
        /// /GS
        /// </remarks>
        True, // NOTE(v.matushkin): Should be lower case true
    }

    public enum ControlFlowGuard
    {
        /// <summary>
        /// Enable Security Check with Guard
        /// </summary>
        /// <remarks>
        /// /guard:cf
        /// </remarks>
        Guard,
        False, // NOTE(v.matushkin): Should be lower case false
    }

    public enum EnableEnhancedInstructionSet
    {
        /// <remarks>
        /// /arch:SSE
        /// </remarks>
        StreamingSIMDExtensions,
        /// <remarks>
        /// /arch:SSE2
        /// </remarks>
        StreamingSIMDExtensions2,
        /// <remarks>
        /// /arch:AVX
        /// </remarks>
        AdvancedVectorExtensions,
        /// <remarks>
        /// /arch:AVX2
        /// </remarks>
        AdvancedVectorExtensions2,
        /// <remarks>
        /// /arch:AVX512
        /// </remarks>
        AdvancedVectorExtensions512,
        /// <remarks>
        /// /arch:IA32
        /// </remarks>
        NoExtensions,
        NotSet,
    }

    public enum FloatingPointModel
    {
        /// <summary>
        /// Default. Improves the consistency of floating-point tests for equality and inequality.
        /// </summary>
        /// <remarks>
        /// /fp:precise
        /// </remarks>
        Precise,
        /// <summary>
        /// The strictest floating-point model. Causes fp_contract to be OFF and fenv_access to be ON. /fp:except is implied and can be disabled by explicitly specifying /fp:except-. When used with /fp:except-, enforces strict floating-point semantics but without respect for exceptional events.
        /// </summary>
        /// <remarks>
        /// /fp:strict
        /// </remarks>
        Strict,
        /// <summary>
        /// Creates the fastest code in the majority of cases.
        /// </summary>
        /// <remarks>
        /// /fp:fast
        /// </remarks>
        Fast,
    }

    public enum SpectreMitigation
    {
        /// <summary>
        /// Enable Spectre mitigation feature for CVE 2017-5753
        /// </summary>
        /// <remarks>
        /// /Qspectre
        /// </remarks>
        Spectre,
        /// <summary>
        /// Enable Spectre mitigations for all load instructions
        /// </summary>
        /// <remarks>
        /// /Qspectre-load
        /// </remarks>
        SpectreLoad,
        /// <summary>
        /// Enable Spectre mitigations for all control flow load instructions
        /// </summary>
        /// <remarks>
        /// /Qspectre-load-cf
        /// </remarks>
        SpectreLoadCF,
        /// <summary>
        /// Disabled. Not set.
        /// </summary>
        False, // NOTE(v.matushkin): Should be lower case false
    }

    #endregion Code Generation


    #region Language

    public enum LanguageStandard
    {
        /// <summary>
        /// Default (ISO C++14 Standard)
        /// </summary>
        Default,
        /// <remarks>
        /// /std:c++14
        /// </remarks>
        stdcpp14,
        /// <remarks>
        /// /std:c++17
        /// </remarks>
        stdcpp17,
        /// <remarks>
        /// /std:c++20
        /// </remarks>
        stdcpp20,
        /// <summary>
        /// Preview - Features from the Latest C++ Working Draft
        /// </summary>
        /// <remarks>
        /// /std:c++latest
        /// </remarks>
        stdcpplatest,
    }

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
        stdc11,
        /// <summary>
        /// ISO C17 (2018) Standard
        /// </summary>
        /// <remarks>
        /// /std:c17
        /// </remarks>
        stdc17,
    }

    public enum ConformanceMode
    {
        Default,
        /// <remarks>
        /// /permissive-
        /// </remarks>
        True, // NOTE(v.matushkin): Should be lower case true
        /// <remarks>
        /// /permissive
        /// </remarks>
        False, // NOTE(v.matushkin): Should be lower case false
    }

    #endregion Language


    #region Precompiled Header

    public enum PrecompiledHeader
    {
        /// <summary>
        /// Instructs the compiler to create a precompiled header (.pch) file that represents the state of compilation at a certain point.
        /// </summary>
        /// <remarks>
        /// /Yc
        /// </remarks>
        Create,
        /// <summary>
        /// Instructs the compiler to use an existing precompiled header (.pch) file in the current compilation.
        /// </summary>
        /// <remarks>
        /// /Yu
        /// </remarks>
        Use,
        /// <summary>
        /// Not Using Precompiled Headers
        /// </summary>
        NotUsing,
    }

    #endregion Precompiled Header


    #region External Includes

    public enum ExternalWarningLevel
    {
        /// <remarks>
        /// /external:W0
        /// </remarks>
        TurnOffAllWarnings,
        /// <remarks>
        /// /external:W1
        /// </remarks>
        Level1,
        /// <remarks>
        /// /external:W2
        /// </remarks>
        Level2,
        /// <remarks>
        /// /external:W3
        /// </remarks>
        Level3,
        /// <remarks>
        /// /external:W4
        /// </remarks>
        Level4,
        /// <summary>
        /// Inherit Project Warning Level
        /// </summary>
        InheritWarningLevel,
    }

    #endregion External Includes


    #region Advanced

    public enum CallingConvention
    {
        /// <summary>
        /// Specifies the __cdecl calling convention for all functions except C++ member functions and functions marked __stdcall or __fastcall.
        /// </summary>
        /// <remarks>
        /// /Gd
        /// </remarks>
        Cdecl,
        /// <summary>
        /// Specifies the __fastcall calling convention for all functions except C++ member sfunctions and functions marked __cdecl or __stdcall. All __fastcall functions must have prototypes.
        /// </summary>
        /// <remarks>
        /// /Gr
        /// </remarks>
        FastCall,
        /// <summary>
        /// Specifies the __stdcall calling convention for all functions except C++ member functions and functions marked __cdecl or __fastcall. All __stdcall functions must have prototypes.
        /// </summary>
        /// <remarks>
        /// /Gz
        /// </remarks>
        StdCall,
        /// <summary>
        /// Specifies the __vectorcall calling convention for all functions except C++ member functions and functions marked __cdecl, __fastcall, or __stdcall. All __vectorcall functions must have prototypes.
        /// </summary>
        /// <remarks>
        /// /Gv
        /// </remarks>
        VectorCall,
    }

    #endregion Advanced
}
