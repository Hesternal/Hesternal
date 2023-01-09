namespace Hesternal.BuildTool;


public static partial class Msvc
{
    #region General

    [CompilerFlag.Enum]
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
        [CompilerFlag.EnumOption("/VERBOSE")]
        LinkVerbose,
        /// <summary>
        /// Displays progress messages indicating just the libraries searched.
        /// </summary>
        /// <remarks>
        /// /VERBOSE:Lib
        /// </remarks>
        [CompilerFlag.EnumOption("/VERBOSE:Lib")]
        LinkVerboseLib,
        /// <summary>
        /// Displays information about COMDAT folding during optimized linking.
        /// </summary>
        /// <remarks>
        /// /VERBOSE:ICF
        /// </remarks>
        [CompilerFlag.EnumOption("/VERBOSE:ICF")]
        LinkVerboseICF,
        /// <summary>
        /// Displays information about functions and data removed during optimized linking.
        /// </summary>
        /// <remarks>
        /// /VERBOSE:REF
        /// </remarks>
        [CompilerFlag.EnumOption("/VERBOSE:REF")]
        LinkVerboseREF,
        /// <summary>
        /// Displays information about modules incompatible with Safe Exception Handling.
        /// </summary>
        /// <remarks>
        /// /VERBOSE:SAFESEH
        /// </remarks>
        [CompilerFlag.EnumOption("/VERBOSE:SAFESEH")]
        LinkVerboseSAFESEH,
        /// <summary>
        /// Display information about linker activity related to managed code.
        /// </summary>
        /// <remarks>
        /// /VERBOSE:CLR
        /// </remarks>
        [CompilerFlag.EnumOption("/VERBOSE:CLR")]
        LinkVerboseCLR,
    }

    [CompilerFlag.Enum]
    public enum ForceFileOutput
    {
        /// <summary>
        /// /FORCE with no arguments implies both multiple and unresolved.
        /// </summary>
        /// <remarks>
        /// /FORCE
        /// </remarks>
        [CompilerFlag.EnumOption("/FORCE")]
        Enabled,
        /// <summary>
        /// Use to create an output file whether or not LINK finds more than one definition for a symbol.
        /// </summary>
        /// <remarks>
        /// /FORCE:MULTIPLE
        /// </remarks>
        [CompilerFlag.EnumOption("/FORCE:MULTIPLE")]
        MultiplyDefinedSymbolOnly,
        /// <summary>
        /// Use to create an output file whether or not LINK finds an undefined symbol. Ignored if the entry point symbol is unresolved.
        /// </summary>
        /// <remarks>
        /// /FORCE:UNRESOLVED
        /// </remarks>
        [CompilerFlag.EnumOption("/FORCE:UNRESOLVED")]
        UndefinedSymbolOnly,
    }

    [CompilerFlag.Enum]
    public enum CreateHotPatchableImage
    {
        /// <summary>
        /// Prepares an image for hotpatching.
        /// </summary>
        /// <remarks>
        /// /FUNCTIONPADMIN
        /// </remarks>
        [CompilerFlag.EnumOption("/FUNCTIONPADMIN")]
        Enabled,
        /// <summary>
        /// Prepares an X86 image for hotpatching.
        /// </summary>
        /// <remarks>
        /// /FUNCTIONPADMIN:5
        /// </remarks>
        [CompilerFlag.EnumOption("/FUNCTIONPADMIN:5")]
        X86Image,
        /// <summary>
        /// Prepares an X64 image for hotpatching.
        /// </summary>
        /// <remarks>
        /// /FUNCTIONPADMIN:6
        /// </remarks>
        [CompilerFlag.EnumOption("/FUNCTIONPADMIN:6")]
        X64Image,
        /// <summary>
        /// Prepares an Itanium image for hotpatching.
        /// </summary>
        /// <remarks>
        /// /FUNCTIONPADMIN:16
        /// </remarks>
        [CompilerFlag.EnumOption("/FUNCTIONPADMIN:16")]
        ItaniumImage,
    }

    #endregion General


    #region Debugging

    [CompilerFlag.Enum]
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
        [CompilerFlag.EnumOption("/DEBUG")]
        True, // NOTE(v.matushkin): Should be lower case true
        /// <summary>
        /// Generate Debug Information optimized for faster links. Produces a program database (PDB) ideal for edit-link-debug cycle.
        /// </summary>
        /// <remarks>
        /// /DEBUG:FASTLINK
        /// </remarks>
        [CompilerFlag.EnumOption("/DEBUG:FASTLINK")]
        DebugFastLink,
        /// <summary>
        /// Generate Debug Information optimized for sharing and publishing. Produces a program database (PDB) ideal for edit-link-debug cycle.
        /// </summary>
        /// <remarks>
        /// /DEBUG:FULL
        /// </remarks>
        [CompilerFlag.EnumOption("/DEBUG:FULL")]
        DebugFull,
    }

    #endregion Debugging


    #region System

    [CompilerFlag.Enum]
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
        [CompilerFlag.EnumOption("/SUBSYSTEM:CONSOLE")]
        Console,
        /// <summary>
        /// Application does not require a console, probably because it creates its own windows for interaction with the user.
        /// If WinMain or wWinMain is defined, WINDOWS is the default.
        /// </summary>
        /// <remarks>
        /// /SUBSYSTEM:WINDOWS
        /// </remarks>
        [CompilerFlag.EnumOption("/SUBSYSTEM:WINDOWS")]
        Windows,
        /// <summary>
        /// Device drivers for Windows NT. If /DRIVER:WDM is specified, NATIVE is the default.
        /// </summary>
        /// <remarks>
        /// /SUBSYSTEM:NATIVE
        /// </remarks>
        [CompilerFlag.EnumOption("/SUBSYSTEM:NATIVE")]
        Native,
        /// <remarks>
        /// /SUBSYSTEM:EFI_APPLICATION
        /// </remarks>
        [CompilerFlag.EnumOption("/SUBSYSTEM:EFI_APPLICATION", "EFI Application")]
        EFI_Application,
        /// <remarks>
        /// /SUBSYSTEM:EFI_BOOT_SERVICE_DRIVER
        /// </remarks>
        [CompilerFlag.EnumOption("/SUBSYSTEM:EFI_BOOT_SERVICE_DRIVER", "EFI Boot")]
        EFI_Boot,
        /// <remarks>
        /// /SUBSYSTEM:EFI_ROM
        /// </remarks>
        [CompilerFlag.EnumOption("/SUBSYSTEM:EFI_ROM", "EFI ROM")]
        EFI_ROM,
        /// <remarks>
        /// /SUBSYSTEM:EFI_RUNTIME_DRIVER
        /// </remarks>
        [CompilerFlag.EnumOption("/SUBSYSTEM:EFI_RUNTIME_DRIVER", "EFI Runtime")]
        EFI_Runtime,
        /// <summary>
        /// Application that runs on a Windows CE device.
        /// </summary>
        /// <remarks>
        /// /SUBSYSTEM:WINDOWSCE
        /// </remarks>
        [CompilerFlag.EnumOption("/SUBSYSTEM:WINDOWSCE")]
        WindowsCE,
        /// <summary>
        /// Application that runs with the POSIX subsystem in Windows NT.
        /// </summary>
        /// <remarks>
        /// /SUBSYSTEM:POSIX
        /// </remarks>
        [CompilerFlag.EnumOption("/SUBSYSTEM:POSIX")]
        POSIX,
    }

    #endregion System


    #region Optimization

    [CompilerFlag.Enum]
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
        [CompilerFlag.EnumOption("/LTCG:incremental")]
        UseFastLinkTimeCodeGeneration,
        /// <summary>
        /// Use Link Time Code Generation.
        /// </summary>
        /// <remarks>
        /// /LTCG
        /// </remarks>
        [CompilerFlag.EnumOption("/LTCG")]
        UseLinkTimeCodeGeneration,
        /// <summary>
        /// Profile Guided Optimization - Instrument. Specifies link-time code generation.
        /// </summary>
        /// <remarks>
        /// /LTCG:PGInstrument
        /// </remarks>
        [CompilerFlag.EnumOption("/LTCG:PGInstrument")]
        PGInstrument,
        /// <summary>
        /// Profile Guided Optimization - Optimization.
        /// Specifies that the linker should use the profile data created after running the instrumented binary to create an optimized image.
        /// </summary>
        /// <remarks>
        /// /LTCG:PGOptimize
        /// </remarks>
        [CompilerFlag.EnumOption("/LTCG:PGOptimize")]
        PGOptimization,
        /// <summary>
        /// Profile Guided Optimization - Update.
        /// Allows and tracks list of input files to be added or modified from what was specified in the :PGINSTRUMENT phase.
        /// </summary>
        /// <remarks>
        /// /LTCG:PGUpdate
        /// </remarks>
        [CompilerFlag.EnumOption("/LTCG:PGUpdate")]
        PGUpdate,
    }

    #endregion Optimization


    #region Windows Metadata

    [CompilerFlag.Enum]
    public enum GenerateWindowsMetadata
    {
        /// <summary>
        /// The default setting for Universal Windows Platform apps.
        /// The linker generates both the binary executable file and the .winmd metadata file.
        /// </summary>
        /// <remarks>
        /// /WINMD
        /// </remarks>
        [CompilerFlag.EnumOption("/WINMD")]
        True, // NOTE(v.matushkin): Should be lower case true
        /// <summary>
        /// The linker generates only the binary executable file, but not a .winmd file.
        /// </summary>
        /// <remarks>
        /// /WINMD:NO
        /// </remarks>
        [CompilerFlag.EnumOption("/WINMD:NO")]
        False, // NOTE(v.matushkin): Should be lower case false
        /// <summary>
        /// The linker generates only the .winmd file, but not the binary executable file.
        /// </summary>
        /// <remarks>
        /// /WINMD:ONLY
        /// </remarks>
        [CompilerFlag.EnumOption("/WINMD:ONLY")]
        Only,
    }

    #endregion Windows Metadata


    #region Advanced

    [CompilerFlag.Enum]
    public enum TargetMachine
    {
        NotSet, // NOTE(v.matushkin): Librarian doesn't have this
        /// <remarks>
        /// /MACHINE:ARM
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:ARM")]
        MachineARM,
        /// <remarks>
        /// /MACHINE:ARM64
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:ARM64")]
        MachineARM64,
        /// <remarks>
        /// /MACHINE:ARM64EC
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:ARM64EC")]
        MachineARM64EC, // NOTE(v.matushkin): Librarian doesn't have this
        /// <remarks>
        /// /MACHINE:ARM64X
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:ARM64X")]
        MachineARM64X,
        /// <remarks>
        /// /MACHINE:EBC
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:EBC")]
        MachineEBC,
        /// <remarks>
        /// /MACHINE:IA64
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:IA64")]
        MachineIA64,
        /// <remarks>
        /// /MACHINE:MIPS
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:MIPS")]
        MachineMIPS,
        /// <remarks>
        /// /MACHINE:MIPS16
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:MIPS16")]
        MachineMIPS16,
        /// <remarks>
        /// /MACHINE:MIPSFPU
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:MIPSFPU")]
        MachineMIPSFPU,
        /// <remarks>
        /// /MACHINE:MIPSFPU16
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:MIPSFPU16")]
        MachineMIPSFPU16,
        /// <remarks>
        /// /MACHINE:SH4
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:SH4")]
        MachineSH4,
        /// <remarks>
        /// /MACHINE:THUMB
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:THUMB")]
        MachineTHUMB,
        /// <remarks>
        /// /MACHINE:X64
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:X64")]
        MachineX64,
        /// <remarks>
        /// /MACHINE:X86
        /// </remarks>
        [CompilerFlag.EnumOption("/MACHINE:X86")]
        MachineX86,
    }

    #endregion Advanced
}
