namespace Copium.BuildTool;


public static partial class Msvc
{
    #region General

    public enum ShowProgress
    {
        /// <summary>
        /// No verbosity.
        /// </summary>
        NotSet,
        /// <summary>
        /// Displays all progress messages.
        /// </summary>
        /// <remarks>
        /// /VERBOSE
        /// </remarks>
        LinkVerbose,
        /// <summary>
        /// Displays progress messages indicating just the libraries searched.
        /// </summary>
        /// <remarks>
        /// /VERBOSE:Lib
        /// </remarks>
        LinkVerboseLib,
        /// <summary>
        /// Displays information about COMDAT folding during optimized linking.
        /// </summary>
        /// <remarks>
        /// /VERBOSE:ICF
        /// </remarks>
        LinkVerboseICF,
        /// <summary>
        /// Displays information about functions and data removed during optimized linking.
        /// </summary>
        /// <remarks>
        /// /VERBOSE:REF
        /// </remarks>
        LinkVerboseREF,
        /// <summary>
        /// Displays information about modules incompatible with Safe Exception Handling.
        /// </summary>
        /// <remarks>
        /// /VERBOSE:SAFESEH
        /// </remarks>
        LinkVerboseSAFESEH,
        /// <summary>
        /// Display information about linker activity related to managed code.
        /// </summary>
        /// <remarks>
        /// /VERBOSE:CLR
        /// </remarks>
        LinkVerboseCLR,
    }

    public enum ForceFileOutput
    {
        /// <summary>
        /// /FORCE with no arguments implies both multiple and unresolved.
        /// </summary>
        /// <remarks>
        /// /FORCE
        /// </remarks>
        Enabled,
        /// <summary>
        /// Use to create an output file whether or not LINK finds more than one definition for a symbol.
        /// </summary>
        /// <remarks>
        /// /FORCE:MULTIPLE
        /// </remarks>
        MultiplyDefinedSymbolOnly,
        /// <summary>
        /// Use to create an output file whether or not LINK finds an undefined symbol. Ignored if the entry point symbol is unresolved.
        /// </summary>
        /// <remarks>
        /// /FORCE:UNRESOLVED
        /// </remarks>
        UndefinedSymbolOnly,
    }

    public enum CreateHotPatchableImage
    {
        /// <summary>
        /// Prepares an image for hotpatching.
        /// </summary>
        /// <remarks>
        /// /FUNCTIONPADMIN
        /// </remarks>
        Enabled,
        /// <summary>
        /// Prepares an X86 image for hotpatching.
        /// </summary>
        /// <remarks>
        /// /FUNCTIONPADMIN:5
        /// </remarks>
        X86Image,
        /// <summary>
        /// Prepares an X64 image for hotpatching.
        /// </summary>
        /// <remarks>
        /// /FUNCTIONPADMIN:6
        /// </remarks>
        X64Image,
        /// <summary>
        /// Prepares an Itanium image for hotpatching.
        /// </summary>
        /// <remarks>
        /// /FUNCTIONPADMIN:16
        /// </remarks>
        ItaniumImage,
    }

    #endregion General


    #region Debugging

    public enum GenerateDebugInformation
    {
        /// <summary>
        /// Produces no debugging information.
        /// </summary>
        False, // NOTE(v.matushkin): Should be lower case false
        /// <summary>
        /// Generate Debug Information. Create a complete Program Database (PDB) ideal for distribution to Microsoft Symbol Server.
        /// </summary>
        /// <remarks>
        /// /DEBUG
        /// </remarks>
        True, // NOTE(v.matushkin): Should be lower case true
        /// <summary>
        /// Generate Debug Information optimized for faster links. Produces a program database (PDB) ideal for edit-link-debug cycle.
        /// </summary>
        /// <remarks>
        /// /DEBUG:FASTLINK
        /// </remarks>
        DebugFastLink,
        /// <summary>
        /// Generate Debug Information optimized for sharing and publishing. Produces a program database (PDB) ideal for edit-link-debug cycle.
        /// </summary>
        /// <remarks>
        /// /DEBUG:FULL
        /// </remarks>
        DebugFull,
    }

    #endregion Debugging


    #region System

    public enum SubSystem
    {
        /// <summary>
        /// No subsystem set.
        /// </summary>
        NotSet, // NOTE(v.matushkin): Librarian doesn't have this
        /// <summary>
        /// Win32 character-mode application. Console applications are given a console by the operating system.
        /// If main or wmain is defined, CONSOLE is the default.
        /// </summary>
        /// <remarks>
        /// /SUBSYSTEM:CONSOLE
        /// </remarks>
        Console,
        /// <summary>
        /// Application does not require a console, probably because it creates its own windows for interaction with the user.
        /// If WinMain or wWinMain is defined, WINDOWS is the default.
        /// </summary>
        /// <remarks>
        /// /SUBSYSTEM:WINDOWS
        /// </remarks>
        Windows,
        /// <summary>
        /// Device drivers for Windows NT. If /DRIVER:WDM is specified, NATIVE is the default.
        /// </summary>
        /// <remarks>
        /// /SUBSYSTEM:NATIVE
        /// </remarks>
        Native,
        /// <remarks>
        /// /SUBSYSTEM:EFI_APPLICATION
        /// </remarks>
        // EFI Application
        /// <remarks>
        /// /SUBSYSTEM:EFI_BOOT_SERVICE_DRIVER
        /// </remarks>
        // EFI Boot
        /// <remarks>
        /// /SUBSYSTEM:EFI_ROM
        /// </remarks>
        // EFI ROM
        /// <remarks>
        /// /SUBSYSTEM:EFI_RUNTIME_DRIVER
        /// </remarks>
        // EFI Runtime
        /// <summary>
        /// Application that runs on a Windows CE device.
        /// </summary>
        /// <remarks>
        /// /SUBSYSTEM:WINDOWSCE
        /// </remarks>
        WindowsCE,
        /// <summary>
        /// Application that runs with the POSIX subsystem in Windows NT.
        /// </summary>
        /// <remarks>
        /// /SUBSYSTEM:POSIX
        /// </remarks>
        POSIX,
    }

    #endregion System


    #region Optimization

    public enum LinkTimeCodeGeneration
    {
        /// <summary>
        /// Default LTCG setting.
        /// </summary>
        Default,
        /// <summary>
        /// Use Fast Link Time Code Generation
        /// </summary>
        /// <remarks>
        /// /LTCG:incremental
        /// </remarks>
        UseFastLinkTimeCodeGeneration,
        /// <summary>
        /// Use Link Time Code Generation.
        /// </summary>
        /// <remarks>
        /// /LTCG
        /// </remarks>
        UseLinkTimeCodeGeneration,
        /// <summary>
        /// Profile Guided Optimization - Instrument. Specifies link-time code generation.
        /// </summary>
        /// <remarks>
        /// /LTCG:PGInstrument
        /// </remarks>
        PGInstrument,
        /// <summary>
        /// Profile Guided Optimization - Optimization.
        /// Specifies that the linker should use the profile data created after running the instrumented binary to create an optimized image.
        /// </summary>
        /// <remarks>
        /// /LTCG:PGOptimize
        /// </remarks>
        PGOptimization,
        /// <summary>
        /// Profile Guided Optimization - Update.
        /// Allows and tracks list of input files to be added or modified from what was specified in the :PGINSTRUMENT phase.
        /// </summary>
        /// <remarks>
        /// /LTCG:PGUpdate
        /// </remarks>
        PGUpdate,
    }

    #endregion Optimization


    #region Windows Metadata

    public enum GenerateWindowsMetadata
    {
        /// <summary>
        /// The default setting for Universal Windows Platform apps.
        /// The linker generates both the binary executable file and the .winmd metadata file.
        /// </summary>
        /// <remarks>
        /// /WINMD
        /// </remarks>
        True, // NOTE(v.matushkin): Should be lower case true
        /// <summary>
        /// The linker generates only the binary executable file, but not a .winmd file.
        /// </summary>
        /// <remarks>
        /// /WINMD:NO
        /// </remarks>
        False, // NOTE(v.matushkin): Should be lower case false
        /// <summary>
        /// The linker generates only the .winmd file, but not the binary executable file.
        /// </summary>
        /// <remarks>
        /// /WINMD:ONLY
        /// </remarks>
        Only,
    }

    #endregion Windows Metadata


    #region Advanced

    public enum TargetMachine
    {
        NotSet, // NOTE(v.matushkin): Librarian doesn't have this
        /// <remarks>
        /// /MACHINE:ARM
        /// </remarks>
        MachineARM,
        /// <remarks>
        /// /MACHINE:ARM64
        /// </remarks>
        MachineARM64,
        /// <remarks>
        /// /MACHINE:ARM64EC
        /// </remarks>
        MachineARM64EC, // NOTE(v.matushkin): Librarian doesn't have this
        /// <remarks>
        /// /MACHINE:ARM64X
        /// </remarks>
        MachineARM64X,
        /// <remarks>
        /// /MACHINE:EBC
        /// </remarks>
        MachineEBC,
        /// <remarks>
        /// /MACHINE:IA64
        /// </remarks>
        MachineIA64,
        /// <remarks>
        /// /MACHINE:MIPS
        /// </remarks>
        MachineMIPS,
        /// <remarks>
        /// /MACHINE:MIPS16
        /// </remarks>
        MachineMIPS16,
        /// <remarks>
        /// /MACHINE:MIPSFPU
        /// </remarks>
        MachineMIPSFPU,
        /// <remarks>
        /// /MACHINE:MIPSFPU16
        /// </remarks>
        MachineMIPSFPU16,
        /// <remarks>
        /// /MACHINE:SH4
        /// </remarks>
        MachineSH4,
        /// <remarks>
        /// /MACHINE:THUMB
        /// </remarks>
        MachineTHUMB,
        /// <remarks>
        /// /MACHINE:X64
        /// </remarks>
        MachineX64,
        /// <remarks>
        /// /MACHINE:X86
        /// </remarks>
        MachineX86,
    }

    #endregion Advanced
}
