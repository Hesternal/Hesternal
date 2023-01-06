using Copium.BuildTool;
using Copium.Core.Net7;


internal sealed class CopiumMath : CopiumCppProject
{
    public CopiumMath(DirectoryItem outputDir)
        : base("Copium.Math", outputDir)
    {
        SetConfigurationMatrix(new CopiumCppConfigurationMatrix(OutputType.StaticLib));

        AddPublicProjectReference<CopiumCore>();

        AddSourcesDir();
    }
}
