using Copium.BuildTool;
using Copium.Core.Net7;


internal sealed class CopiumEngine : CopiumCppProject
{
    public CopiumEngine(DirectoryItem outputDir)
        : base("CopiumEngine", outputDir)
    {
        BaseGeneratedIncludeDir = "EngineCht";

        SetConfigurationMatrix(new CopiumCppConfigurationMatrix(OutputType.StaticLib));

        AddPublicProjectReference<CopiumCore>();
        AddPublicProjectReference<CopiumECS>();
        AddPublicProjectReference<CopiumMath>();

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


    protected override void OnConfigureCopiumProject(CopiumCppConfiguration configuration)
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
