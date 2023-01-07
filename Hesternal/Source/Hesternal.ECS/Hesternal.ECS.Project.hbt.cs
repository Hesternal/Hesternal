using Hesternal.BuildTool;
using Hesternal.Core.Net7;


internal sealed class HesternalECS : HesternalCppProject
{
    public HesternalECS(DirectoryItem outputDir)
        : base("Hesternal.ECS", outputDir)
    {
        SetConfigurationMatrix(new HesternalCppConfigurationMatrix(OutputType.StaticLib));

        AddPublicProjectReference<HesternalCore>();
        AddPublicProjectReference<HesternalMath>();

        AddSourcesDir();
    }


    protected override void OnConfigureHesternalProject(HesternalCppConfiguration configuration)
    {
        configuration.PublicConanLibs.Add(ConanLibrary.entt);
    }
}
