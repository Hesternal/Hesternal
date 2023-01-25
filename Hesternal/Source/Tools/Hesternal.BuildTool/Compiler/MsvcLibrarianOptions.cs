using static Hesternal.BuildTool.Old.Msvc;

namespace Hesternal.BuildTool.Old;


public sealed class MsvcLibrarianOptions : ILinkerOptions
{
    /// <summary>
    /// The /NOLOGO option prevents display of the copyright message and version number.
    /// </summary>
    /// <remarks>
    /// <see langword="true"/> /NOLOGO
    /// </remarks>
    [CompilerFlag.Bool("/NOLOGO")]
    public bool SuppressStartupBanner = true;

    /// <summary>
    /// The /NODEFAULTLIB option tells the lib to remove one or more default libraries from the list of libraries it searches when resolving external references.
    /// </summary>
    /// <remarks>
    /// <see langword="true"/> /NODEFAULTLIB
    /// </remarks>
    [CompilerFlag.Bool("/NODEFAULTLIB")]
    public bool? IgnoreAllDefaultLibraries = null;

    /// <summary>
    /// Use UNICODE Response Files : Instructs the project system to generate UNICODE response files when spawning the librarian.
    /// Set this property to 'Yes' when files in the project have UNICODE paths.
    /// </summary>
    public bool? UseUnicodeResponseFiles = null;

    /// <summary>
    /// Specifies whether or not library outputs from project dependencies are automatically linked in.
    /// </summary>
    public bool LinkLibraryDependencies = false;

    /// <summary>
    /// /WX causes no output file to be generated if lib generates a warning.
    /// </summary>
    /// <remarks>
    /// <see langword="true"/> /WX, <see langword="false"/> /WX:NO
    /// </remarks>
    [CompilerFlag.Bool("/WX", "/WX:NO")]
    public bool? TreatLibWarningAsErrors = null;

    /// <summary>
    /// The /MACHINE option specifies the target platform for the program.
    /// </summary>
    public TargetMachine TargetMachine = TargetMachine.MachineX64;

    /// <summary>
    /// The /SUBSYSTEM option tells the operating system how to run the .exe file.
    /// The choice of subsystem affects the entry point symbol (or entry point function) that the linker will choose.
    /// </summary>
    public SubSystem? SubSystem = null;

    public bool? Verbose = null;

    /// <summary>
    /// Specifies link-time code generation.
    /// </summary>
    /// <remarks>
    /// <see langword="true"/> /LTCG
    /// </remarks>
    [CompilerFlag.Bool("/LTCG")]
    public bool? LinkTimeCodeGeneration = null;
}
