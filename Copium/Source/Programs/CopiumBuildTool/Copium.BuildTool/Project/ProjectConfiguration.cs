using System;

using Copium.BuildTool.BuildRules;

namespace Copium.BuildTool.Project;


internal abstract class ProjectConfiguration
{
    public readonly BuildOS OS;
    public readonly BuildPlatform Platform;
    public readonly BuildConfiguration Configuration;

    public readonly string PlatformName;
    public readonly string ConfigurationName;


    protected ProjectConfiguration(
        BuildOS os,
        BuildPlatform platform,
        BuildConfiguration configuration,
        string configurationName
        )
    {
        OS = os;
        Platform = platform;
        Configuration = configuration;
        PlatformName = platform.ToString();
        ConfigurationName = configurationName;
    }


    // public static ProjectConfiguration Csharp(
    //     BuildPlatform platform,
    //     BuildCompiler compiler,
    //     BuildArchitecture architecture,
    //     BuildConfiguration configuration)
    // {
    //     return new ProjectConfiguration(platform, compiler, architecture, configuration, configuration.ToString());
    // }
    // 
    // public static ProjectConfiguration Cpp(
    //     BuildPlatform platform,
    //     BuildCompiler compiler,
    //     BuildArchitecture architecture,
    //     BuildConfiguration configuration)
    // {
    //     string configurationName = $"{compiler.ToString().ToLower()}-{configuration.ToString().ToLower()}";
    //     return new ProjectConfiguration(platform, compiler, architecture, configuration, configurationName);
    // }
}


internal class CsharpProjectConfiguration : ProjectConfiguration
{
    public CsharpProjectConfiguration(
        BuildOS os,
        BuildPlatform platform,
        BuildConfiguration configuration
)
        : base(os, platform, configuration, _MakeCsharpConfigurationName(configuration))
    {
    }


    private static string _MakeCsharpConfigurationName(BuildConfiguration configuration)
    {
        return configuration.ToString();
    }
}


internal class CppProjectConfiguration : ProjectConfiguration
{
    public readonly BuildCompiler Compiler;


    public CppProjectConfiguration(
        BuildOS os,
        BuildCompiler compiler,
        BuildPlatform platform,
        BuildConfiguration configuration
)
        : base(os, platform, configuration, _MakeCppConfigurationName(compiler, configuration))
    {
        if (platform == BuildPlatform.AnyCPU)
        {
            throw new ArgumentException("Cpp project configuration doesn't support BuildPlatform.AnyCPU");
        }

        Compiler = compiler;
    }


    private static string _MakeCppConfigurationName(BuildCompiler compiler, BuildConfiguration configuration)
    {
        return compiler.ToString().ToLower() + "-" + configuration.ToString().ToLower();
    }
}
