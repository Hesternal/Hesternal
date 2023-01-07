using System;
using System.Collections.Generic;

using Hesternal.CSharp.Net7;

namespace Hesternal.BuildTool.Generators;


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
