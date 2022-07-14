using System.Collections.Generic;

using Copium.BuildTool.Build;
using Copium.BuildTool.Core;

namespace Copium.BuildTool.Project;


internal sealed class TargetCollection
{
    public readonly List<RulesTarget> RulesTargets = new();
    public readonly List<CppTarget> CppTargets = new();
    public FileItem[]? ProgramSolutionFiles;
}
