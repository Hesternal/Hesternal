using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

using Copium.Core.Net7;

namespace Copium.BuildTool;


public abstract class Solution
{
    public readonly string Name;
    public readonly DirectoryItem OutputDir;

    public IReadOnlyCollection<CppProject> CppProjects => m_cppProjects.Values;


    /// <summary>
    /// .cbt.cs file
    /// </summary>
    protected readonly FileItem MakeFile;
    /// <summary>
    /// .cbt.cs file directory
    /// </summary>
    protected readonly DirectoryItem MakeFileDir;


    private readonly Dictionary<Type, CppProject> m_cppProjects = new();


    protected Solution(string solutionName, DirectoryItem outputDir, [CallerFilePath] string solutionFilePath = "")
    {
        Name = solutionName;
        OutputDir = outputDir;

        MakeFile = new FileItem(solutionFilePath);
        MakeFileDir = MakeFile.Directory;
    }


    public void ResolveDependencies()
    {
        foreach (CppProject cppProject in m_cppProjects.Values)
        {
            cppProject.Resolve(this);
        }
    }

    public CppProject TypeToProject(Type type) => m_cppProjects[type];


    protected void AddProject<TProject>(TProject project)
        where TProject : CppProject
    {
        m_cppProjects.Add(typeof(TProject), project);
    }
}
