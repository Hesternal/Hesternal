using Copium.BuildTool;
using Copium.Core.Net7;


internal sealed class HesternalEngine : HesternalCppProject
{
    public HesternalEngine(DirectoryItem outputDir)
        : base("HesternalEngine", outputDir)
    {
        BaseGeneratedIncludeDir = "EngineCht";

        SetConfigurationMatrix(new HesternalCppConfigurationMatrix(OutputType.StaticLib));

        AddPublicProjectReference<HesternalCore>();
        AddPublicProjectReference<HesternalECS>();
        AddPublicProjectReference<HesternalMath>();

        AddSourcesDir("Assets");
        AddSourcesDir("Core");
        AddSourcesDir("ECS");
        AddSourcesDir("Engine");
        AddSourcesDir("Event");
        AddSourcesDir("Graphics");
        AddSourcesDir("ImGui");
        AddSourcesDir("Memory");
        AddSourcesDir("Platform");
        AddSourcesDir("Utils");
    }


    protected override void OnConfigureHesternalProject(HesternalCppConfiguration configuration)
    {
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
