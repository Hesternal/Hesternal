using System;
using System.Collections.Generic;

using Copium.BuildTool.Core;

namespace Copium.BuildTool.Project;


internal abstract class SolutionFile
{
    private readonly FileItem m_projectFile;
    protected readonly DirectoryItem m_solutionDir;

    public ReadOnlySpan<char> Name => m_projectFile.Name;


    protected SolutionFile(DirectoryItem solutionDir, FileItem solutionFile)
    {
        m_solutionDir = solutionDir;
        m_projectFile = solutionFile;
    }


    protected IEnumerable<string> ReadLines() => m_projectFile.ReadLines();
}
