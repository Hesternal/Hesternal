namespace Copium.BuildTool.BuildRules;


public enum BuildOS
{
    Windows,
}

public enum BuildCompiler
{
    Msvc,
}

public enum BuildPlatform
{
    x64,
    AnyCPU,
}

public enum BuildConfiguration
{
    Debug,
    Development,
    Release,
}


public sealed class TargetBuildInfo
{
    public readonly BuildOS OS;
    public readonly BuildCompiler Compiler;

    public TargetBuildInfo(BuildOS os, BuildCompiler compiler)
    {
        OS = os;
        Compiler = compiler;
    }
}
