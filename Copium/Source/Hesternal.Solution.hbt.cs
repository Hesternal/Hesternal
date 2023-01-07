using Hesternal.BuildTool;
using Hesternal.Core.Net7;


internal sealed class HesternalSolution : Solution
{
    public HesternalSolution()
        : base("Hesternal", Globals.RootDir, Globals.Build.ProjectDir.MakeSubDirItem("vs"))
    {
        AddProject(new HesternalCore(OutputDir));
        AddProject(new HesternalECS(OutputDir));
        AddProject(new HesternalMath(OutputDir));
        AddProject(new HesternalEngine(OutputDir));
        AddProject(new HesternalEngineTests(OutputDir));
        AddProject(new HesternalEditor(OutputDir));
    }
}
