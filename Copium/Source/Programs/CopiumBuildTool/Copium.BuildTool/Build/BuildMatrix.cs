using Copium.BuildTool.BuildRules;

namespace Copium.BuildTool.Build;


internal sealed class BuildMatrix
{
    public readonly BuildOS OS = BuildOS.Windows;
    public readonly BuildCompiler Compiler = BuildCompiler.Msvc;
    public readonly BuildPlatform Platform = BuildPlatform.x64;
    public readonly BuildConfiguration[] Configurations = new[]
    {
        BuildConfiguration.Debug,
        BuildConfiguration.Development,
        BuildConfiguration.Release,
    };
}
