using Copium.Core.Net7;

namespace Copium.BuildTool.Generators.VisualStudio;


internal abstract class VsProject<T>
    where T : Project
{
    public readonly T Base;
    public readonly string ProjectGuid;
    public readonly FileItem OutputFile;

    public abstract string ProjectTypeGuid { get; }


    protected VsProject(T project, string projectFileExtension)
    {
        Base = project;
        ProjectGuid = VisualStudioGuid.Generate();
        OutputFile = project.OutputDir.MakeSubFileItem(project.Name + projectFileExtension);
    }
}
