using Copium.BuildTool;
using Copium.Core.Net7;


internal sealed class HesternalEngineTests : HesternalCppTestsProject
{
    public HesternalEngineTests(DirectoryItem outputDir)
        : base("HesternalEngine.Tests", outputDir)
    {
        SetConfigurationMatrix(new HesternalCppConfigurationMatrix(OutputType.Executable));

        AddPrivateProjectReference<HesternalEngine>();

        AddSourcesDir("Math");
    }
}
