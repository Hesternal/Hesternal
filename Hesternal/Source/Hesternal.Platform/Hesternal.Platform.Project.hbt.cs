using Hesternal.BuildTool;
using Hesternal.CSharp.Net7;


internal sealed class HesternalPlatform : HesternalCppProject
{
    public HesternalPlatform(DirectoryItem outputDir)
        : base("Hesternal.Platform", outputDir)
    {
        SetConfigurationMatrix(new HesternalCppConfigurationMatrix(OutputType.StaticLib));

        AddPublicProjectReference<HesternalCore>();

        AddSourcesDir();
    }
}
