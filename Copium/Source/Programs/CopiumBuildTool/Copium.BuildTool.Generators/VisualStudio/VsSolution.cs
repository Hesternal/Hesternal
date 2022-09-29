using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;

namespace Copium.BuildTool.Generators.VisualStudio;


internal sealed class VsSolution
{
    public readonly Solution Base;

    public readonly ReadOnlyCollection<VsCppProject> CppProjects;


    public VsSolution(Solution solution)
    {
        Base = solution;

        Dictionary<CppProject, VsCppProject> cppToVsProject = new();

        foreach (CppProject cppProject in solution.CppProjects)
        {
            cppToVsProject.Add(cppProject, new VsCppProject(cppProject));
        }

        CppProjects = cppToVsProject.Values.ToArray().AsReadOnly();

        foreach (VsCppProject vsCppProject in CppProjects)
        {
            vsCppProject.Resolve(cppToVsProject);
        }
    }
}
