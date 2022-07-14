using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

using Copium.BuildTool.Core;

namespace Copium.BuildTool.Project.VisualStudio;


internal sealed class VsSolutionFile : SolutionFile
{
    private static class ProjectRegexGroup
    {
        public const int ProjectTypeGuid = 1;
        public const int ProjectName     = 1 + ProjectTypeGuid;
        public const int ProjectPath     = 1 + ProjectName;
        public const int ProjectGuid     = 1 + ProjectPath;
    }

    private static readonly Lazy<Regex> s_ProjectRegex = new(() => new Regex(
               @"^Project\(\""([^\""]+)\""\) = \""([^\""]+)\"", \""([^\""]+)\"", \""([^\""]+)\""$",
               RegexOptions.Compiled | RegexOptions.NonBacktracking
           ),
           false);


    public readonly VsCsharpProjectFile[] CsharpProjects;


    public VsSolutionFile(FileItem solutionFile)
        : base(solutionFile.Directory, solutionFile)
    {
        CsharpProjects = _ParseSolutionFile();
    }


    private VsCsharpProjectFile[] _ParseSolutionFile()
    {
        List<VsCsharpProjectFile> csharpProjects = new();

        Regex projectRegex = s_ProjectRegex.Value;

        foreach (string line in ReadLines())
        {
            // No more projects if we reached Global section
            if (line == "Global")
            {
                break;
            }

            Match regexMatch = projectRegex.Match(line);

            if (regexMatch.Success && regexMatch.Groups[ProjectRegexGroup.ProjectTypeGuid].Value == VisualStudioGuid.Csproj)
            {
                string projectName = regexMatch.Groups[ProjectRegexGroup.ProjectName].Value;
                string projectGuid = regexMatch.Groups[ProjectRegexGroup.ProjectGuid].Value;
                string solutionRelativeProjectPath = regexMatch.Groups[ProjectRegexGroup.ProjectPath].Value;
                FileItem projectFile = m_solutionDir.MakeSubFileItem(solutionRelativeProjectPath);

                csharpProjects.Add(new VsCsharpProjectFile(projectFile.Directory, projectFile, projectName, projectGuid));
            }
        }

        return csharpProjects.ToArray();
    }
}
