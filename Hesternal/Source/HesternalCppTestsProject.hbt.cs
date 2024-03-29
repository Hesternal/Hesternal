using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;

using Hesternal.BuildTool;
using Hesternal.CSharp.Net7;


internal abstract class HesternalCppTestsProject : HesternalCppBaseProject
{
    protected HesternalCppTestsProject(string projectName, DirectoryItem outputDir, [CallerFilePath] string projectFilePath = "")
        : base(projectName, outputDir, projectFilePath)
    {
    }


    protected sealed override void OnConfigureHesternalProject(HesternalCppConfiguration configuration)
    {
        configuration.PrivateConanLibs.Add(ConanLibrary.catch2);
    }


    protected void AddSourcesDir(string dirPath)
    {
        DirectoryItem sourcesDir = MakeFileDir.MakeSubDirItem(dirPath);
        AddSources(sourcesDir.EnumerateFiles(SearchPattern.Sources));
    }
}
