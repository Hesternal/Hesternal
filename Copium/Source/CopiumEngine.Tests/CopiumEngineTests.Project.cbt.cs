using Copium.BuildTool;
using Copium.Core.Net7;


internal sealed class CopiumEngineTests : CopiumCppTestsProject
{
    public CopiumEngineTests(DirectoryItem outputDir)
        : base("CopiumEngine.Tests", outputDir)
    {
        SetConfigurationMatrix(new CopiumCppConfigurationMatrix(OutputType.Executable));

        AddPrivateProjectReference<CopiumEngine>();

        AddSourcesDir("Math");
    }
}
