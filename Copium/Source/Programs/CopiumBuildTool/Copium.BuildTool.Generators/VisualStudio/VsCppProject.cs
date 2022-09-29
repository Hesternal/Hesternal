using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace Copium.BuildTool.Generators.VisualStudio;


internal sealed class VsCppProject : VsProject<CppProject>
{
    public ReadOnlyCollection<VsCppProject>? PrivateProjectReferences { get; private set; }
    public ReadOnlyCollection<VsCppProject>? PublicProjectReferences { get; private set; }

    public ReadOnlyCollection<VsCppProjectConfiguration> Configurations => m_configurations;

    public readonly VsPropsFile? PrivateProps;
    public readonly VsPropsFile? PublicProps;

    public override string ProjectTypeGuid => VisualStudioGuid.Vcxproj;


    private readonly ReadOnlyCollection<VsCppProjectConfiguration> m_configurations;


    public VsCppProject(CppProject project)
        : base(project, VsFileExtension.Vcxproj)
    {
        bool usesPropsFiles = false;

        VsCppProjectConfiguration[] vsConfigurations = new VsCppProjectConfiguration[project.Configurations.Count];

        for (int i = 0; i < project.Configurations.Count; i++)
        {
            CppProjectConfiguration cppProjectConfiguration = project.Configurations[i];
            vsConfigurations[i] = new VsCppProjectConfiguration(cppProjectConfiguration);

            if (cppProjectConfiguration.OutputType == OutputType.StaticLib)
            {
                usesPropsFiles = true;
            }
        }

        m_configurations = vsConfigurations.AsReadOnly();

        if (usesPropsFiles)
        {
            PrivateProps = new(project.OutputDir.MakeSubFileItem(project.Name + VsFileExtension.PropsPrivate), project.OutputDir);
            PublicProps = new(project.OutputDir.MakeSubFileItem(project.Name + VsFileExtension.PropsPublic), project.OutputDir);
        }
        else
        {
            PrivateProps = null;
            PublicProps = null;
        }
    }


    public void Resolve(Dictionary<CppProject, VsCppProject> cppToVsProject)
    {
        PrivateProjectReferences = _ResolveProjectReferences(Base.PrivateProjectReferences, cppToVsProject);
        PublicProjectReferences = _ResolveProjectReferences(Base.PublicProjectReferences, cppToVsProject);
    }


    // NOTE(v.matushkin): Hmm, I think I've seen this code somewhere
    private static ReadOnlyCollection<VsCppProject>? _ResolveProjectReferences(ReadOnlyCollection<CppProject>? references, Dictionary<CppProject, VsCppProject> cppToVsProject)
    {
        if (references is null or { Count: 0 })
        {
            return null;
        }

        int referenceCount = references.Count;
        VsCppProject[] projectReferences = new VsCppProject[referenceCount];

        for (int i = 0; i < referenceCount; i++)
        {
            projectReferences[i] = cppToVsProject[references[i]];
        }

        return projectReferences.AsReadOnly();
    }
}
