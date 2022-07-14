using Copium.BuildTool.BuildRules;

namespace Copium.BuildTool.Project.VisualStudio;


// internal sealed class VsProjectConfiguration : ProjectConfiguration
// {
//     public readonly string ArchitectureName;
//     /// <summary>
//     /// Vcxproj specific shit, like "Debug|x64", platform is actually means architecture
//     /// </summary>
//     public readonly string ConfigurationArch;
//     /// <summary>
//     /// "'$(Configuration)|$(Platform)'=='Debug|x64'" string
//     /// </summary>
//     public readonly string Condition;
// 
// 
//     private VsProjectConfiguration(
//         BuildPlatform platform,
//         BuildCompiler compiler,
//         BuildArchitecture architecture,
//         BuildConfiguration configuration
//         )
//         : base(platform, compiler, architecture, configuration)
//     {
//         ArchitectureName = architecture.ToString();
//         ConfigurationArch = $"{ConfigurationName}|{ArchitectureName}";
//         Condition = $"'$(Configuration)|$(Platform)'=='{ConfigurationArch}'";
//     }
// 
// 
//     public static new ProjectConfiguration Csharp(
//         BuildPlatform platform,
//         BuildCompiler compiler,
//         BuildArchitecture architecture,
//         BuildConfiguration configuration)
//     {
//         return new ProjectConfiguration(platform, compiler, architecture, configuration, configuration.ToString());
//     }
// 
//     public static new ProjectConfiguration Cpp(
//         BuildPlatform platform,
//         BuildCompiler compiler,
//         BuildArchitecture architecture,
//         BuildConfiguration configuration)
//     {
//         string configurationName = $"{compiler.ToString().ToLower()}-{configuration.ToString().ToLower()}";
//         return new ProjectConfiguration(platform, compiler, architecture, configuration, configurationName);
//     }
// }


internal interface IVsProjectConfiguration
{
    /// <summary>
    /// Visual Studio project Configuration|Platform, like "Debug|x64"
    /// </summary>
    public string ConfigurationPlatform { get; }
    /// <summary>
    /// "'$(Configuration)|$(Platform)'=='Debug|x64'" string
    /// </summary>
    public string Condition { get; }
}


internal sealed class VsCsharpProjectConfiguration : CsharpProjectConfiguration, IVsProjectConfiguration
{
    private readonly string m_configurationPlatform;
    private readonly string m_condition;

    public string ConfigurationPlatform => m_configurationPlatform;
    public string Condition => m_condition;


    public VsCsharpProjectConfiguration(
        BuildOS os,
        BuildPlatform platform,
        BuildConfiguration configuration
        )
        : base(os, platform, configuration)
    {
        m_configurationPlatform = ConfigurationName + "|" + PlatformName;
        m_condition = $"'$(Configuration)|$(Platform)'=='{m_configurationPlatform}'";
    }
}


internal sealed class VsCppProjectConfiguration : CppProjectConfiguration, IVsProjectConfiguration
{
    private readonly string m_configurationPlatform;
    private readonly string m_condition;

    public string ConfigurationPlatform => m_configurationPlatform;
    public string Condition => m_condition;


    public VsCppProjectConfiguration(
        BuildOS os,
        BuildCompiler compiler,
        BuildPlatform platform,
        BuildConfiguration configuration
        )
        : base(os, compiler, platform, configuration)
    {
        m_configurationPlatform = ConfigurationName + "|" + PlatformName;
        m_condition = $"'$(Configuration)|$(Platform)'=='{m_configurationPlatform}'";
    }
}
