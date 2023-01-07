using System;
using System.Collections.Generic;

namespace Copium.BuildTool;


public class CppProjectConfiguration : ProjectConfiguration
{
    public readonly BuildCompiler Compiler;

    /// <summary>
    /// Path to where the compiled program will be placed
    /// </summary>
    public string OutputDir = string.Empty;
    /// <summary>
    /// Path to where the intermediate build files get generated
    /// </summary>
    public string IntermediateDir = string.Empty;

    public readonly List<ConanLibrary> PrivateConanLibs = new();
    public readonly List<ConanLibrary> PublicConanLibs = new();
    public readonly List<string> PrivateSystemLibs = new();

    public readonly List<string> PublicDefines = new();

    public readonly IBuildSystemOptions BuildSystemOptions;
    public readonly ICompilerOptions CompilerOptions;
    public readonly ILinkerOptions LinkerOptions;

    public readonly List<string> AdditionalCompilerOptions = new();


    public CppProjectConfiguration(CppConfigurationDesc desc)
        : base(desc)
    {
        if (OutputType != OutputType.Executable && OutputType != OutputType.SharedLib && OutputType != OutputType.StaticLib)
        {
            throw new ArgumentException("Cpp project configuration supports only Executable and Static/Shared libs OutputType");
        }

        Compiler = desc.Compiler;

        BuildSystemOptions = new MSBuildOptions();

        switch (desc.Compiler)
        {
            case BuildCompiler.Msvc:
                CompilerOptions = new MsvcOptions();
                LinkerOptions = OutputType == OutputType.StaticLib ? new MsvcLibrarianOptions() : new MsvcLinkerOptions();
                break;
            default:
                throw new NotSupportedException("Only MSVC compiler supported for now");
        }
    }
}
