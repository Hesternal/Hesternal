using System;
using System.IO;
using System.Linq;

//using Copium.BuildTool;
//using Copium.BuildTool.Core;
using Copium.Core.Net7;

namespace Copium.BuildTool.Generators.VisualStudio;


public sealed class VsSolutionGenerator : IDisposable
{
    // private sealed class VsSolutionFolder
    // {
    //     public readonly string Guid;
    //     public readonly List<VsSolutionFolder> ChildFolders = new();
    //     public readonly List<string> ProjectGuids = new();
    // 
    //     public VsSolutionFolder()
    //     {
    //         Guid = VisualStudioGuid.Generate();
    //     }
    // }

    private static readonly string[] s_WhateverThisIs = { "ActiveCfg", "Build.0" };

    private readonly VsSolution m_solution;

    private readonly DirectoryItem m_solutionDir;
    private readonly FileItem m_solutionFile;

    private readonly VsGenerationContext m_generationContext;

    private readonly StreamWriter m_solutionWriter;


    public static void Generate(Solution solution)
    {
        using VsSolutionGenerator generator = new(solution);
        generator._GenerateSolution();
    }


    private VsSolutionGenerator(Solution solution)
    {
        m_solution = new VsSolution(solution);

        m_solutionDir = solution.OutputDir;
        m_solutionDir.Create();
        m_solutionFile = m_solutionDir.MakeSubFileItem(solution.Name + VsFileExtension.Solution);

        m_generationContext = new VsGenerationContext(m_solutionDir);

        m_solutionWriter = new(m_solutionFile.OpenWrite());
    }

    public void Dispose() => m_solutionWriter.Dispose();


    private void _GenerateSolution()
    {
        foreach (VsCppProject cppProject in m_solution.CppProjects)
        {
            VsCppProjectGenerator.Generate(m_generationContext, cppProject);
        }

        _GenerateSolutionFile();
    }


    private void _GenerateSolutionFile()
    {
        //string programsFolderGuid = VisualStudioGuid.Generate();
        //string rulesFolderGuid = VisualStudioGuid.Generate();

        // NOTE(v.matushkin): Need a proper SolutionFolder class instead of this abomination
        //(string FolderGuid, VsSolutionFile Solution)[] programSolutionFolders = m_generationContext.ProgramSolutions == null
        //    ? Array.Empty<(string, VsSolutionFile)>()
        //    : m_generationContext.ProgramSolutions.Select(solution => (VisualStudioGuid.Generate(), solution)).ToArray();

        _Header();

        //- Solution projects

        //-- Cpp Targets
        foreach (VsCppProject cppProject in m_solution.CppProjects)
        {
            _WriteProject(cppProject);
        }
        //-- Csharp Targets
        //foreach (VsCsharpProjectFile externalCsharpProject in m_generationContext.CSharpProjectFIles)
        //{
        //    _WriteProject(externalCsharpProject);
        //}
        //-- Folders
        //_WriteProjectFolder("Pograms", programsFolderGuid);
        //_WriteProjectFolder("Rules", rulesFolderGuid);
        //foreach (var (FolderGuid, Solution) in programSolutionFolders)
        //{
        //    _WriteProjectFolder(Solution.Name, FolderGuid);
        //}

        //- GlobalSection
        m_solutionWriter.WriteLine("Global");

        //-- SolutionConfigurationPlatforms
        // GlobalSection(SolutionConfigurationPlatforms) = preSolution
        //     Debug|x64 = Debug|x64
        //     Release|x64 = Release|x64
        //     Final|x64 = Final|x64
        // EndGlobalSection
        m_solutionWriter.WriteLine("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution");
        // TODO(v.matushkin): HACK
        foreach (CppProjectConfiguration projectConfiguration in m_solution.CppProjects[0].Base.Configurations)
        {
            _WriteSolutionConfiguration(projectConfiguration);
        }
        m_solutionWriter.WriteLine("\tEndGlobalSection");

        //-- ProjectConfigurationPlatforms
        // GlobalSection(ProjectConfigurationPlatforms) = postSolution
        //     {9E9CC5C0-23B8-32DB-838E-1758F5D7E73B}.Debug|x64.ActiveCfg = Debug|x64
        //     {9E9CC5C0-23B8-32DB-838E-1758F5D7E73B}.Debug|x64.Build.0 = Debug|x64
        //     {9E9CC5C0-23B8-32DB-838E-1758F5D7E73B}.Development|x64.ActiveCfg = Development|x64
        //     {9E9CC5C0-23B8-32DB-838E-1758F5D7E73B}.Development|x64.Build.0 = Development|x64
        //     {9E9CC5C0-23B8-32DB-838E-1758F5D7E73B}.Release|x64.ActiveCfg = Release|x64
        //     {9E9CC5C0-23B8-32DB-838E-1758F5D7E73B}.Release|x64.Build.0 = Release|x64
        // EndGlobalSection
        m_solutionWriter.WriteLine("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution");
        foreach (VsCppProject cppProject in m_solution.CppProjects)
        {
            foreach (CppProjectConfiguration projectConfiguration in cppProject.Base.Configurations)
            {
                foreach (string whateverThisIs in s_WhateverThisIs)
                {
                    _WriteProjectConfiguration(
                        cppProject.ProjectGuid,
                        projectConfiguration,
                        whateverThisIs,
                        projectConfiguration);
                }
            }
        }
        //foreach (VsCsharpProjectFile csharpProject in m_generationContext.CSharpProjectFIles)
        //{
        //    foreach (VsCppProjectConfiguration projectConfiguration in m_generationContext.CppProjectConfigurations)
        //    {
        //        foreach (string whateverThisIs in s_WhateverThisIs)
        //        {
        //            _WriteProjectConfigurationPlatform(
        //                csharpProject.ProjectGuid,
        //                projectConfiguration.ConfigurationPlatform,
        //                whateverThisIs,
        //                m_generationContext.CsharpProjectConfiguration.ConfigurationPlatform);
        //        }
        //    }
        //}
        m_solutionWriter.WriteLine("\tEndGlobalSection");

        //-- SolutionProperties
        // GlobalSection(SolutionProperties) = preSolution
        //     HideSolutionNode = TRUE
        // EndGlobalSection
        m_solutionWriter.WriteLine("\tGlobalSection(SolutionProperties) = preSolution");
        m_solutionWriter.WriteLine("\t\tHideSolutionNode = FALSE");
        m_solutionWriter.WriteLine("\tEndGlobalSection");

        //-- NestedProjects
        // GlobalSection(NestedProjects) = preSolution
        //     {CHILD_GUID} = {PARENT_GUID}
        // EndGlobalSection
        m_solutionWriter.WriteLine("\tGlobalSection(NestedProjects) = preSolution");

        //--- Program folders
        //foreach (var (FolderGuid, Solution) in programSolutionFolders)
        //{
        //    // Program Solution folder -> Programs folder
        //    _WriteNestedProject(programsFolderGuid, FolderGuid);
        //
        //    foreach (VsCsharpProjectFile csharpProjectFile in Solution.CsharpProjectFiles)
        //    {
        //        // Solution project -> Program Solution folder
        //        _WriteNestedProject(FolderGuid, csharpProjectFile.ProjectGuid);
        //    }
        //}

        //--- Rules folder
        //foreach (VsCsharpProjectFile csharpProject in m_generationContext.RulesProjects)
        //{
        //    //--- Rules project -> Rules folder
        //    _WriteNestedProject(rulesFolderGuid, csharpProject.ProjectGuid);
        //}

        m_solutionWriter.WriteLine("\tEndGlobalSection");

        _ExtensibilityGlobals();

        m_solutionWriter.WriteLine("EndGlobal");
    }


    private void _Header()
    {
        // TODO(v.matushkin): Get VisualStudio version at runtime?
        // solutionWriter.WriteLine();
        m_solutionWriter.WriteLine("Microsoft Visual Studio Solution File, Format Version 12.00");
        m_solutionWriter.WriteLine("# Visual Studio Version 17");
        m_solutionWriter.WriteLine("VisualStudioVersion = 17.3.32519.111");
        m_solutionWriter.WriteLine("MinimumVisualStudioVersion = 10.0.40219.1");
    }

    // GlobalSection(ExtensibilityGlobals) = postSolution
    //     SolutionGuid = {BD915904-0D2B-4A58-A304-7E7E4AEDE110}
    // EndGlobalSection
    private void _ExtensibilityGlobals()
    {
        m_solutionWriter.WriteLine("\tGlobalSection(ExtensibilityGlobals) = postSolution");
        m_solutionWriter.Write("\t\tSolutionGuid = ");
        m_solutionWriter.WriteLine(VisualStudioGuid.Generate());
        m_solutionWriter.WriteLine("\tEndGlobalSection");
    }


    //private void _WriteProjectFolder(ReadOnlySpan<char> name, string guid)
    //{
    //    _WriteProject(VisualStudioGuid.SolutionFolder, name, name, guid);
    //}

    private void _WriteProject<TProject>(VsProject<TProject> project)
        where TProject : Project
    {
        _WriteProject(project.ProjectTypeGuid, project.Base.Name, project.OutputFile.GetRelativePath(m_solutionDir), project.ProjectGuid);
    }

    // Project("{PROJECT_TYPE_GUID}") = "PROJECT_NAME", "PROJECT_PATH", "{PROJECT_GUID}"
    // EndProject
    private void _WriteProject(string typeGuid, ReadOnlySpan<char> projectName, ReadOnlySpan<char> projectPath, string projectGuid)
    {
        m_solutionWriter.Write("Project(\"");
        m_solutionWriter.Write(typeGuid);
        m_solutionWriter.Write("\") = \"");
        m_solutionWriter.Write(projectName);
        m_solutionWriter.Write("\", \"");
        m_solutionWriter.Write(projectPath);
        m_solutionWriter.Write("\", \"");
        m_solutionWriter.Write(projectGuid);
        m_solutionWriter.WriteLine('"');
        m_solutionWriter.WriteLine("EndProject");
    }

    // \t\tCONFIGURATION|PLATFORM = CONFIGURATION|PLATFORM
    private void _WriteSolutionConfiguration(CppProjectConfiguration configuration)
    {
        m_solutionWriter.Write("\t\t");
        m_solutionWriter.Write(configuration.Name);
        m_solutionWriter.Write('|');
        m_solutionWriter.Write(configuration.PlatformString);
        m_solutionWriter.Write(" = ");
        m_solutionWriter.Write(configuration.Name);
        m_solutionWriter.Write('|');
        m_solutionWriter.WriteLine(configuration.PlatformString);
    }

    // \t\t{PROJECT_GUID}.SOLUTION_CONFIGURATION_PLATFORM.WhateverThisIs = PROJECT_CONFIGURATION_PLATFORM
    private void _WriteProjectConfiguration(
        string projectGuid,
        CppProjectConfiguration solutionConfiguration,
        string whateverThisIs,
        CppProjectConfiguration projectConfiguration
        )
    {
        m_solutionWriter.Write("\t\t");
        m_solutionWriter.Write(projectGuid);
        m_solutionWriter.Write('.');
        m_solutionWriter.Write(solutionConfiguration.Name);
        m_solutionWriter.Write('|');
        m_solutionWriter.Write(solutionConfiguration.PlatformString);
        m_solutionWriter.Write('.');
        m_solutionWriter.Write(whateverThisIs);
        m_solutionWriter.Write(" = ");
        m_solutionWriter.Write(projectConfiguration.Name);
        m_solutionWriter.Write('|');
        m_solutionWriter.WriteLine(projectConfiguration.PlatformString);
    }

    // \t\t{CHILD_GUID} = {PARENT_GUID}
    private void _WriteNestedProject(string parentGuid, string childGuid)
    {
        m_solutionWriter.Write("\t\t");
        m_solutionWriter.Write(childGuid);
        m_solutionWriter.Write(" = ");
        m_solutionWriter.WriteLine(parentGuid);
    }
}
