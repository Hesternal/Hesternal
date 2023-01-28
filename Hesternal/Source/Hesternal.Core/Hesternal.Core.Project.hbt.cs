using Hesternal.BuildTool;
using Hesternal.CSharp.Net7;


internal sealed class HesternalCore : HesternalCppProject
{
    public HesternalCore(DirectoryItem outputDir)
        : base("Hesternal.Core", outputDir)
    {
        BaseGeneratedIncludeDir = "HesternalCoreCht";

        SetConfigurationMatrix(new HesternalCppConfigurationMatrix(OutputType.StaticLib));

        AddSourcesDir();
    }


    protected override void OnConfigureHesternalProject(HesternalCppConfiguration configuration)
    {
        if (configuration.OS == BuildOS.Windows)
        {
            configuration.PublicDefines.AddRange(new[] {
                "WIN32_LEAN_AND_MEAN",
                "UNICODE",
                "_UNICODE",
                "_CRT_SECURE_NO_WARNINGS",
                "HS_PLATFORM_WINDOWS",
            });
        }

        if (configuration.Compiler == BuildCompiler.Msvc)
        {
            configuration.PublicDefines.Add("HS_COMPILER_MSVC");
        }

        switch (configuration.Configuration)
        {
            case BuildConfiguration.Debug:
                configuration.PublicDefines.Add("HS_CONFIGURATION_DEBUG");
                break;
            case BuildConfiguration.Development:
                configuration.PublicDefines.Add("HS_CONFIGURATION_DEVELOPMENT");
                break;
            case BuildConfiguration.Release:
                configuration.PublicDefines.Add("HS_CONFIGURATION_RELEASE");
                break;
        }

        configuration.PublicConanLibs.Add(ConanLibrary.spdlog);
    }
}
