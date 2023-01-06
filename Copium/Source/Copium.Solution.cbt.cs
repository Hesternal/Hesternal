using Copium.BuildTool;
using Copium.Core.Net7;


internal sealed class CopiumSolution : Solution
{
    public CopiumSolution()
        : base("Copium", Globals.RootDir, Globals.Build.ProjectDir.MakeSubDirItem("vs"))
    {
        AddProject(new CopiumCore(OutputDir));
        AddProject(new CopiumECS(OutputDir));
        AddProject(new CopiumMath(OutputDir));
        AddProject(new CopiumEngine(OutputDir));
        AddProject(new CopiumEngineTests(OutputDir));
        AddProject(new CopiumEditor(OutputDir));
    }
}
