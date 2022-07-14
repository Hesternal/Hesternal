using Copium.BuildTool.BuildRules;

public sealed class CopiumEngine : TargetBuildRules
{
    public override TargetType eTargetType => TargetType.Engine;
    public override string BaseGeneratedIncludeDir => "EngineCht";

    public CopiumEngine(TargetBuildInfo targetBuildInfo)
        : base(targetBuildInfo)
    {
        if (OS == BuildOS.Windows)
        {
            PublicDefines.Common.AddRange(new[] {
                "WIN32_LEAN_AND_MEAN",
                "UNICODE",
                "_UNICODE",
                "_CRT_SECURE_NO_WARNINGS",
                "COP_PLATFORM_WINDOWS",
            });
        }

        if (Compiler == BuildCompiler.Msvc)
        {
            PublicDefines.Common.Add("COP_COMPILER_MSVC");
        }

        PublicDefines.Debug.Add("COP_CONFIGURATION_DEBUG");
        PublicDefines.Development.Add("COP_CONFIGURATION_DEVELOPMENT");
        PublicDefines.Release.Add("COP_CONFIGURATION_RELEASE");
    }
}
