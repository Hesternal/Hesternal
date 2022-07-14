using System.Collections.Generic;
using System.Linq;

using Copium.BuildTool.BuildRules;
using Copium.BuildTool.Core;

namespace Copium.BuildTool.Build;


internal sealed class Module
{
    private const string k_PrivateHeadersDirName = "IncludePrivate";
    private const string k_PublicHeadersDirName  = "IncludePublic";
    private const string k_PrivateModulesDirName = "Private";
    private const string k_PublicModulesDirName  = "Public";
    private const string k_SourcesDirName        = "Source";

    public readonly string ModuleName;

    /// <summary>
    /// Private Conan and System libraries
    /// </summary>
    public readonly LibraryDependencies PrivateLibraries;
    /// <summary>
    /// Public Conan and System libraries
    /// </summary>
    public readonly LibraryDependencies PublicLibraries;

    public readonly DirectoryItem? PrivateIncludeDir;
    public readonly DirectoryItem? PublicIncludeDir;
    /// <remark>
    /// For some reason, I need to list every subdir of public module dir in vcxproj PublicModuleDirectories property
    ///  to make it work for the targets consuming libraries with this property.
    /// May be I'm doing something wrong, may be it's a bug, may be this is how it's supposed to be done, idk.
    ///
    /// It does work for cases like this though:
    ///  CopiumEditor
    ///    Some .cpp file:
    ///      import File.ixx
    ///      import FileBinaryConverter.ixx
    ///  CopiumEngine
    ///    PublicModuleDirectories <- vcxproj property
    ///      Public/Core
    ///      Public/Core/Serialization
    ///    Public/                 <- public module dir structure
    ///      Core/
    ///        Platform/
    ///          File.ixx
    ///        Serialization/
    ///          FileBinaryConverter.ixx
    ///            import File.ixx
    ///
    /// Even though I'm not listing Public/Core/Platform dir, 'import File.ixx' works in the CopiumEditor
    ///  because FileBinaryConverter.ixx references it in the CopiumEngine
    /// </remark>
    public readonly DirectoryItem[]? PublicModuleDirs;

    /// <summary>
    /// Private .hpp files
    /// </summary>
    public readonly FileItem[]? PrivateHeaders;
    /// <summary>
    /// Public .hpp files
    /// </summary>
    public readonly FileItem[]? PublicHeaders;
    /// <summary>
    /// Private .ixx files
    /// </summary>
    public readonly FileItem[]? PrivateModules;
    /// <summary>
    /// Public .ixx files
    /// </summary>
    public readonly FileItem[]? PublicModules;
    /// <summary>
    /// .cpp files
    /// </summary>
    public readonly FileItem[]? Sources;


    public Module(DirectoryItem moduleDir, ModuleBuildRules moduleBuildRules)
    {
        ModuleName = moduleDir.Name;
        PrivateLibraries = moduleBuildRules.PrivateLibraries;
        PublicLibraries = moduleBuildRules.PublicLibraries;

        foreach (DirectoryItem moduleSourcesDir in moduleDir.EnumerateDirectories())
        {
            switch (moduleSourcesDir.Name)
            {
                case k_PrivateHeadersDirName:
                    PrivateIncludeDir = moduleSourcesDir;
                    PrivateHeaders = _GetSourceFiles(moduleSourcesDir);
                    break;
                case k_PublicHeadersDirName:
                    PublicIncludeDir = moduleSourcesDir;
                    PublicHeaders = _GetSourceFiles(moduleSourcesDir);
                    break;
                case k_PrivateModulesDirName:
                    PrivateModules = _GetSourceFiles(moduleSourcesDir);
                    break;
                case k_PublicModulesDirName:
                    PublicModules = _GetSourceFiles(moduleSourcesDir);
                    PublicModuleDirs = _GetPublicModuleDirs(moduleSourcesDir, PublicModules);
                    break;
                case k_SourcesDirName:
                    Sources = _GetSourceFiles(moduleSourcesDir);
                    break;
            }
        }
    }


    public IEnumerable<string> EnumerateIncludeDirs()
    {
        if (PrivateIncludeDir != null)
            yield return PrivateIncludeDir.FullPath;

        if (PublicIncludeDir != null)
            yield return PublicIncludeDir.FullPath;
    }


    private static FileItem[]? _GetSourceFiles(DirectoryItem moduleSourcesDir)
    {
        FileItem[] sourceFiles = moduleSourcesDir.EnumerateFiles().ToArray();
        return sourceFiles.Length == 0 ? null : sourceFiles;
    }

    private static DirectoryItem[] _GetPublicModuleDirs(DirectoryItem publicModuleDir, FileItem[]? publicModuleFiles)
    {
        if (publicModuleFiles == null)
        {
            return new[] { publicModuleDir };
        }

        return publicModuleFiles.Select(file => file.Directory)
            .Prepend(publicModuleDir)
            .DistinctBy(dir => dir.FullPath)
            .ToArray()!;
    }


    // public void ApplyModuleRules(ModuleBuildRules[] moduleBuildRules)
    // {
    //     bool DirFilter(FileItem file, HashSet<string> dirFiler)
    //     {
    //         foreach (string publicHeaderDirFilter in dirFiler)
    //         {
    //             if (file.FullPath.StartsWith(publicHeaderDirFilter))
    //             {
    //                 return true;
    //             }
    //         }
    // 
    //         return false;
    //     }
    // 
    //     Microsoft.Extensions.Logging.ILogger logger = Log.CreateLogger(m_targetDir.Name + " CppProject");
    // 
    //     for (int i = 0; i < moduleBuildRules.Length; i++)
    //     {
    //         ModuleBuildRules moduleRules = moduleBuildRules[i];
    //         CppModuleDir cppModuleDir = m_cppModuleDirs[i];
    // 
    //         if (cppModuleDir.PublicHeaders != null)
    //         {
    //             if (moduleRules.PublicHeadersDirectoryFilter.Count != 0)
    //             {
    //                 cppModuleDir.PublicHeaders.RemoveAll(header =>
    //                 {
    //                     foreach (string publicHeaderDirFilter in moduleRules.PublicHeadersDirectoryFilter)
    //                     {
    //                         if (header.FullPath.StartsWith(publicHeaderDirFilter))
    //                         {
    //                             return true;
    //                         }
    //                     }
    // 
    //                     return false;
    //                 });
    //             }
    // 
    //             foreach (FileItem publicHeader in cppModuleDir.PublicHeaders)
    //             {
    //                 
    //             }
    //         }
    //     }
    // 
    //     moduleBuildRules.PublicHeadersDirectoryFilter.
    // 
    //     foreach (string publicHeaderDirPathFilter in moduleBuildRules.PublicHeadersDirectoryFilter)
    //     {
    // 
    //     }
    // }
}
