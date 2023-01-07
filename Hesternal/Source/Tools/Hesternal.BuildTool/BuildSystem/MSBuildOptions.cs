using static Hesternal.BuildTool.MSBuild;

namespace Hesternal.BuildTool;


public sealed class MSBuildOptions : IBuildSystemOptions
{
    public PlatformToolset PlatformToolset = PlatformToolset.v143;


    /// <summary>
    /// Determines which build tools will be used to compile programs (32-bit or 64-bit).
    /// </summary>
    public PreferredToolArchitecture PreferredToolArchitecture = PreferredToolArchitecture.x64;

    /// <summary>
    /// Specifies whether the configuration is set up for development or for production.
    /// </summary>
    public bool UseDebugLibraries = false;

    /// <summary>
    /// Copy items that are marked as content to this project's output directory.
    /// </summary>
    public bool CopyLocalDeploymentContent = false;

    /// <summary>
    /// Copy project references' output (dll/exe) to this project's output directory. In C++/CLI (/clr) projects,
    /// this property is ignored and referenced output copying is controlled by the Copy Local property of the reference.
    /// </summary>
    public bool CopyLocalProjectReference = false;

    /// <summary>
    /// Copy project references' symbols (pdb) to this project's output directory along side to the project references' output (dll/exe).
    /// Always enabled for CLR Projects.
    /// </summary>
    public bool CopyLocalDebugSymbols = false;

    /// <summary>
    /// Copy C++ runtime dlls to this project's output directory.
    /// </summary>
    public bool CopyCppRuntimeToOutputDir = false;

    /// <summary>
    /// Tells the compiler to use the specified character set; aids in localization issues.
    /// </summary>
    public CharacterSet CharacterSet = CharacterSet.NotSet;
}
