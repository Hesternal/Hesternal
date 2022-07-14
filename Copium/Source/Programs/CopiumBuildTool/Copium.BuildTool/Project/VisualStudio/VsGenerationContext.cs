using System;
using System.Collections.Generic;

using Copium.BuildTool.Build;
using Copium.BuildTool.BuildRules;
using Copium.BuildTool.Core;

using Microsoft.Extensions.Logging;

namespace Copium.BuildTool.Project.VisualStudio;


internal sealed class VsGenerationContext
{
    private static class PropsFileName
    {
        public const string DirectoryBuild = "Directory.Build.props";
        public const string CopiumCpp      = "Copium.Cpp.props";
        public const string CopiumCppMsvc  = "Copium.Cpp.Msvc.props";
    }


    private const string k_SubDirName = "vs";

    private readonly Dictionary<string, VsCppProjectFile> m_cppProjects = new();
    private readonly Dictionary<string, VsCsharpProjectFile> m_csharpProjects = new();

    public readonly DirectoryItem SolutionDir;
    private readonly DirectoryItem m_solutionDotnetDir;

    public readonly VsCppProjectConfiguration[] CppProjectConfigurations;
    public readonly FileItem CopiumCppPropsFile;
    public readonly FileItem CopiumCppMsvcPropsFile;

    public readonly VsCsharpProjectConfiguration CsharpProjectConfiguration;

    public readonly VsCsharpProjectFile[] RulesProjects;
    public readonly VsSolutionFile[]? ProgramSolutions;

    public IReadOnlyCollection<VsCppProjectFile> CppProjects => m_cppProjects.Values;
    public IReadOnlyCollection<VsCsharpProjectFile> CsharpProjects => m_csharpProjects.Values;


    public VsGenerationContext(DirectoryItem solutionDir, BuildMatrix buildMatrix, TargetCollection targetCollection)
    {
        SolutionDir = solutionDir;
        m_solutionDotnetDir = solutionDir.MakeSubDirItem("dotnet");
        m_solutionDotnetDir.Create();

        CppProjectConfigurations = _BuildMatrixToConfigurations(buildMatrix);
        _GeneratePropsFile_DirectoryBuild(solutionDir);
        CopiumCppPropsFile = _GeneratePropsFile_CopiumCpp(solutionDir);
        CopiumCppMsvcPropsFile = _GeneratePropsFile_CopiumCppMsvc(solutionDir, CppProjectConfigurations);

        CsharpProjectConfiguration = new(BuildOS.Windows, BuildPlatform.AnyCPU, BuildConfiguration.Release);

        _CreateCppProjectFiles(targetCollection.CppTargets);
        RulesProjects = _CreateRulesProjectFiles(targetCollection.RulesTargets);
        if (targetCollection.ProgramSolutionFiles != null)
        {
            ProgramSolutions = _LoadProgramSolutions(targetCollection.ProgramSolutionFiles);
        }
    }


    public VsCppProjectFile GetCppProject(string projectName)
    {
        if (m_cppProjects.TryGetValue(projectName, out VsCppProjectFile? cppProjectFile))
        {
            return cppProjectFile;
        }

        throw new CbtException($"Trying to get project \"{projectName}\", which doesn't exists or not yet added");
    }

    public VsCsharpProjectFile GetCsharpProject(string projectName)
    {
        if (m_csharpProjects.TryGetValue(projectName, out VsCsharpProjectFile? csharpProjectFile))
        {
            return csharpProjectFile;
        }

        throw new CbtException($"Trying to get project \"{projectName}\", which doesn't exists or not yet added");
    }


    public static string GetConanLibPropsPath(ConanLibrary library)
    {
        return @"$(CopiumConanDir)\conan_" + library.ToString() + VsFileExtension.Props;
    }


    private void _CreateCppProjectFiles(List<CppTarget> cppTargets)
    {
        foreach (CppTarget cppTarget in cppTargets)
        {
            VsCppProjectFile cppProjectFile = new(SolutionDir, cppTarget);
            m_cppProjects.Add(cppProjectFile.ProjectName, cppProjectFile);
        }
    }

    private VsCsharpProjectFile[] _CreateRulesProjectFiles(List<RulesTarget> rulesTargets)
    {
        var charpProjectFiles = new VsCsharpProjectFile[rulesTargets.Count];

        for (int i = 0; i < charpProjectFiles.Length; ++i)
        {
            VsCsharpProjectFile csharpProjectFile = new(m_solutionDotnetDir, rulesTargets[i].TargetName);
            charpProjectFiles[i] = csharpProjectFile;

            m_csharpProjects.Add(csharpProjectFile.ProjectName, csharpProjectFile);
        }

        return charpProjectFiles;
    }

    private VsSolutionFile[] _LoadProgramSolutions(FileItem[] programSolutionFiles)
    {
        List<VsSolutionFile> programSolutions = new();

        foreach (FileItem programSolutionFile in programSolutionFiles)
        {
            VsSolutionFile solution = new(programSolutionFile);

            foreach (VsCsharpProjectFile externalCsharpProject in solution.CsharpProjects)
            {
                m_csharpProjects.Add(externalCsharpProject.ProjectName, externalCsharpProject);
            }

            programSolutions.Add(solution);
        }

        return programSolutions.ToArray();
    }


    private static VsCppProjectConfiguration[] _BuildMatrixToConfigurations(BuildMatrix buildMatrix)
    {
        List<VsCppProjectConfiguration> cppProjectConfigurations = new();

        foreach (BuildConfiguration configuration in buildMatrix.Configurations)
        {
            cppProjectConfigurations.Add(new(buildMatrix.OS, buildMatrix.Compiler, buildMatrix.Platform, configuration));
        }

        return cppProjectConfigurations.ToArray();
    }

    private static void _GeneratePropsFile_DirectoryBuild(DirectoryItem solutionDir)
    {
        using VsXmlWriter propsFileWriter = new(solutionDir.MakeSubFileItem(PropsFileName.DirectoryBuild));

        propsFileWriter.BeginProject(null, null, null, true);

        propsFileWriter.BeginPropertyGroup();
        propsFileWriter.Property("CopiumRootDir", CopiumGlobal.RootDir.FullPath);
        //- Build dirs
        propsFileWriter.Property("CopiumBuildDir", @"$(CopiumRootDir)\build");
        propsFileWriter.Property("CopiumBinDir", @"$(CopiumBuildDir)\bin");
        propsFileWriter.Property("CopiumConanDir", @"$(CopiumBuildDir)\conan\" + k_SubDirName);
        propsFileWriter.Property("CopiumGeneratedDir", @"$(CopiumBuildDir)\generated");
        propsFileWriter.Property("CopiumIntermediateDir", @"$(CopiumBuildDir)\intermediate\" + k_SubDirName);
        //- Source dirs
        propsFileWriter.Property("CopiumConfigDir", @"$(CopiumRootDir)\Copium\Config");
        propsFileWriter.Property("CopiumSourceDir", @"$(CopiumRootDir)\Copium\Source");
        //- Target
        propsFileWriter.Property("CopiumTargetName", "$(MSBuildProjectName)");
        propsFileWriter.Property("CopiumTargetGeneratedDir", @"$(CopiumGeneratedDir)\$(CopiumTargetName)");
        propsFileWriter.Property("CopiumTargetSourceDir", @"$(CopiumSourceDir)\$(CopiumTargetName)");
        propsFileWriter.EndElement(); // BeginPropertyGroup

        propsFileWriter.EndElement(); // BeginProject
    }

    private static FileItem _GeneratePropsFile_CopiumCpp(DirectoryItem solutionDir)
    {
        FileItem propsFile = solutionDir.MakeSubFileItem(PropsFileName.CopiumCpp);

        using VsXmlWriter propsFileWriter = new(propsFile);

        propsFileWriter.BeginProject(null, null, null, true);

        propsFileWriter.BeginItemDefinitionGroup();
        propsFileWriter.BeginItem(VcxprojItem.ClCompile);
        propsFileWriter.Property(VcxprojClCompileItem.AdditionalIncludeDirectories, @"$(CopiumTargetGeneratedDir)\Include;%(AdditionalIncludeDirectories)");
        propsFileWriter.EndElement();
        propsFileWriter.EndElement();

        propsFileWriter.EndElement(); // BeginProject

        return propsFile;
    }

    private static FileItem _GeneratePropsFile_CopiumCppMsvc(DirectoryItem solutionDir, VsCppProjectConfiguration[] cppProjectConfigurations)
    {
        FileItem propsFile = solutionDir.MakeSubFileItem(PropsFileName.CopiumCppMsvc);

        using VsXmlWriter propsFileWriter = new(propsFile);

        void AddCompilerFlags(string[] compilerFlags, Dictionary<string, NameValue> compilerTable)
        {
            foreach (string flag in compilerFlags)
            {
                if (compilerTable.TryGetValue(flag, out var flagNameValue))
                {
                    propsFileWriter.Property(flagNameValue.Name, flagNameValue.Value);
                }
                else
                {
                    Log.Logger.LogError("Couldn't find flag: /{flag}", flag);
                }
            }
        }

        propsFileWriter.BeginProject(null, null, null, true);

        foreach (VsCppProjectConfiguration projectConfiguration in cppProjectConfigurations)
        {
            string[] compilerDefines;
            string[] compilerCppFlags;
            string[] compilerLinkerFlags;

            switch (projectConfiguration.Configuration)
            {
                case BuildConfiguration.Debug:
                    compilerDefines = CopiumGlobal.BuildConfigurations.Msvc.Preprocessor.Debug;
                    compilerCppFlags = CopiumGlobal.BuildConfigurations.Msvc.Cpp.Debug;
                    compilerLinkerFlags = CopiumGlobal.BuildConfigurations.Msvc.Link.Debug;
                    break;
                case BuildConfiguration.Development:
                    compilerDefines = CopiumGlobal.BuildConfigurations.Msvc.Preprocessor.Development;
                    compilerCppFlags = CopiumGlobal.BuildConfigurations.Msvc.Cpp.Development;
                    compilerLinkerFlags = CopiumGlobal.BuildConfigurations.Msvc.Link.Development;
                    break;
                case BuildConfiguration.Release:
                    compilerDefines = CopiumGlobal.BuildConfigurations.Msvc.Preprocessor.Release;
                    compilerCppFlags = CopiumGlobal.BuildConfigurations.Msvc.Cpp.Release;
                    compilerLinkerFlags = CopiumGlobal.BuildConfigurations.Msvc.Link.Release;
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            propsFileWriter.BeginItemDefinitionGroup(projectConfiguration.Condition);

            //- ClCompile
            {
                propsFileWriter.BeginItem(VcxprojItem.ClCompile);
                propsFileWriter.Property(VcxprojClCompileItem.PreprocessorDefinitions, compilerDefines);
                AddCompilerFlags(CopiumGlobal.BuildConfigurations.Msvc.Cpp.Common, VisualStudioConfig.Msvc.Cpp);
                AddCompilerFlags(compilerCppFlags, VisualStudioConfig.Msvc.Cpp);
                propsFileWriter.EndElement();
            }
            //- Link
            {
                propsFileWriter.BeginItem(VcxprojItem.Link);
                AddCompilerFlags(compilerLinkerFlags, VisualStudioConfig.Msvc.Link);
                propsFileWriter.EndElement();
            }

            propsFileWriter.EndElement(); // ItemDefinitionGroup
        }

        propsFileWriter.EndElement(); // BeginProject

        return propsFile;
    }
}
