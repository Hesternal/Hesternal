using Copium.BuildTool;
using Copium.Core.Net7;


internal sealed class CopiumEngine : CopiumCppProject
{
    public CopiumEngine(DirectoryItem outputDir)
        : base("CopiumEngine", outputDir)
    {
        BaseGeneratedIncludeDir = "EngineCht";

        SetConfigurationMatrix(new CopiumCppConfigurationMatrix(OutputType.StaticLib));

        AddSourcesDir("Assets");
        AddSourcesDir("Core");
        AddSourcesDir("ECS");
        AddSourcesDir("Engine");
        AddSourcesDir("Event");
        AddSourcesDir("Graphics");
        AddSourcesDir("ImGui");
        AddSourcesDir("Math");
        AddSourcesDir("Memory");
        AddSourcesDir("Platform");
        AddSourcesDir("Utils");
    }


    protected override void OnConfigureCopiumProject(CopiumCppConfiguration configuration)
    {
        if (configuration.OS == BuildOS.Windows)
        {
            configuration.PublicDefines.AddRange(new[] {
                "WIN32_LEAN_AND_MEAN",
                "UNICODE",
                "_UNICODE",
                "_CRT_SECURE_NO_WARNINGS",
                "COP_PLATFORM_WINDOWS",
            });
        }

        if (configuration.Compiler == BuildCompiler.Msvc)
        {
            configuration.PublicDefines.Add("COP_COMPILER_MSVC");
        }

        switch (configuration.Configuration)
        {
            case BuildConfiguration.Debug:
                configuration.PublicDefines.Add("COP_CONFIGURATION_DEBUG");
                break;
            case BuildConfiguration.Development:
                configuration.PublicDefines.Add("COP_CONFIGURATION_DEVELOPMENT");
                break;
            case BuildConfiguration.Release:
                configuration.PublicDefines.Add("COP_CONFIGURATION_RELEASE");
                break;
        }

        //- Core
        configuration.PrivateConanLibs.Add(ConanLibrary.glfw);
        configuration.PublicConanLibs.Add(ConanLibrary.simdjson);
        configuration.PublicConanLibs.Add(ConanLibrary.spdlog);
        //- ECS
        configuration.PublicConanLibs.Add(ConanLibrary.entt);
        //- Graphics
        if (configuration.OS == BuildOS.Windows)
        {
            configuration.PrivateSystemLibs.Add(SystemLibs.Windows.D3D11);
            configuration.PrivateSystemLibs.Add(SystemLibs.Windows.DXGI);
            configuration.PrivateSystemLibs.Add(SystemLibs.Windows.D3DCompiler);
            // TODO(v.matushkin): This should be added for Debug configuration only ?
            configuration.PrivateSystemLibs.Add(SystemLibs.Windows.DXGuid);
        }
        //- ImGui
        configuration.PublicConanLibs.Add(ConanLibrary.imgui);
    }
}
