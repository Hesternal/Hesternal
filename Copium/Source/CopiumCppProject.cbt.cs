using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;

using Copium.BuildTool;
using Copium.Core.Net7;


internal abstract class CopiumCppProject : CopiumCppBaseProject
{
    private static class DirName
    {
        public const string PrivateHeaders = "IncludePrivate";
        public const string PublicHeaders  = "IncludePublic";
        public const string PrivateModules = "Private";
        public const string PublicModules  = "Public";
        public const string Sources        = "Source";
    }


    protected CopiumCppProject(string projectName, DirectoryItem outputDir, [CallerFilePath] string projectFilePath = "")
        : base(projectName, outputDir, projectFilePath)
    {
    }


    protected void AddSourcesDir(string subDirPath = "")
    {
        DirectoryItem subDir = subDirPath == string.Empty ? MakeFileDir : MakeFileDir.MakeSubDirItem(subDirPath);

        foreach (DirectoryItem sourcesDir in subDir.EnumerateDirectories())
        {
            switch (sourcesDir.Name)
            {
                case DirName.PrivateHeaders:
                    AddPrivateIncludeDir(sourcesDir.FullPath);
                    AddPrivateHeaders(sourcesDir.EnumerateFiles(SearchPattern.Headers));
                    break;
                case DirName.PublicHeaders:
                    AddPublicIncludeDir(sourcesDir.FullPath);
                    AddPublicHeaders(sourcesDir.EnumerateFiles(SearchPattern.Headers));
                    break;
                case DirName.PrivateModules:
                    AddPrivateModules(sourcesDir.EnumerateFiles(SearchPattern.Modules));
                    break;
                case DirName.PublicModules:
                    List<FileItem> publicModules = sourcesDir.EnumerateFiles(SearchPattern.Modules).ToList();
                    AddPublicModuleDirs(_EnumeratePublicModuleDirs(sourcesDir, publicModules));
                    AddPublicModules(publicModules);
                    break;
                case DirName.Sources:
                    AddSources(sourcesDir.EnumerateFiles(SearchPattern.Sources));
                    break;
            }
        }
    }

    private static IEnumerable<string> _EnumeratePublicModuleDirs(DirectoryItem publicModuleDir, List<FileItem> publicModules)
    {
        return publicModules.Select(file => file.Directory.FullPath)
            .Prepend(publicModuleDir.FullPath)
            .Distinct();
    }
}
