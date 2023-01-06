using Copium.BuildTool;
using Copium.Core.Net7;


internal sealed class CopiumECS : CopiumCppProject
{
    public CopiumECS(DirectoryItem outputDir)
        : base("Copium.ECS", outputDir)
    {
        SetConfigurationMatrix(new CopiumCppConfigurationMatrix(OutputType.StaticLib));

        AddPublicProjectReference<CopiumCore>();
        AddPublicProjectReference<CopiumMath>();

        AddSourcesDir();
    }


    protected override void OnConfigureCopiumProject(CopiumCppConfiguration configuration)
    {
        configuration.PublicConanLibs.Add(ConanLibrary.entt);
    }
}
