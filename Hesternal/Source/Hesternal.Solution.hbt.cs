using System.IO;

using Hesternal.BuildTool;
using Hesternal.CSharp.Net7;


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


    // TODO(v.matushkin): <SolutionPostConfigureHack>
    public override void OnPostSolutionConfigure()
    {
        FileItem headerToolPropsFile = Globals.Hesternal.HeaderToolPropsFile;
        FileItem headerToolTargetFile = Globals.Hesternal.HeaderToolTargetFile;
        File.Copy(headerToolPropsFile.FullPath, OutputDir.MakeSubFileItem(headerToolPropsFile.FileName).FullPath, true);
        File.Copy(headerToolTargetFile.FullPath, OutputDir.MakeSubFileItem(headerToolTargetFile.FileName).FullPath, true);
    }
}
