using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;

using Copium.Core.Net7;

namespace Copium.BuildTool.Generators.VisualStudio;


[ProjectGenerator]
[SupportedBuildSystems(typeof(MSBuildOptions))]
[SupportedCompilers(typeof(MsvcOptions))]
internal sealed partial class VsCppProjectGenerator : IDisposable
{
    private static class VcxprojGlobalsPropertyName
    {
        public const string Keyword                      = "Keyword";
        public const string WindowsTargetPlatformVersion = "WindowsTargetPlatformVersion";
    }

    private static class VcxprojGlobalsPropertyValue
    {
        public const string Keyword                      = "Win32Proj";
        public const string WindowsTargetPlatformVersion = "10.0";
    }

    private static class VcxprojImports
    {
        public const string MicrosoftCppDefaultProps      = @"$(VCTargetsPath)\Microsoft.Cpp.Default.props";
        public const string MicrosoftCppProps             = @"$(VCTargetsPath)\Microsoft.Cpp.props";
        public const string MicrosoftCppPlatformUserProps = @"$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props";
        public const string MicrosoftCppTargets           = @"$(VCTargetsPath)\Microsoft.Cpp.targets";
    }

    private static class HeaderToolTask
    {
        public const string PropsFileName  = "Copium.Cpp.HeaderTool.props";
        public const string TargetFileName = "Copium.Cpp.HeaderTool.targets";
    }

    private const string k_ToolsVersion   = "Current";
    private const string k_DefaultTargets = "Build";


    private readonly VsGenerationContext m_generationContext;
    private readonly VsCppProject m_project;

    private readonly ReadOnlyCollection<VsCppProject>? m_privateProjectReferences;
    private readonly ReadOnlyCollection<VsCppProject>? m_publicProjectReferences;
    private readonly bool m_hasPrivateReferences;
    private readonly bool m_hasPublicReferences;

    //private readonly VsCppProjectFile? m_referenceProjectFile;

    //private readonly VsCppProjectFile m_projectFile;
    //private DirectoryItem ProjectDir => m_projectFile.ProjectDir;
    private readonly FileItem m_filtersFile;

    private readonly VsXmlWriter m_projectWriter;


    public static void Generate(VsGenerationContext generationContext, VsCppProject cppProject)
    {
        using VsCppProjectGenerator generator = new(generationContext, cppProject);
        generator._GenerateProject();
        generator._GenerateFiltersFile();
    }


    private VsCppProjectGenerator(VsGenerationContext generationContext, VsCppProject cppProject)
    {
        m_generationContext = generationContext;
        m_project = cppProject;

        m_privateProjectReferences = cppProject.PrivateProjectReferences;
        m_publicProjectReferences = cppProject.PublicProjectReferences;
        m_hasPrivateReferences = m_privateProjectReferences is { Count: > 0 };
        m_hasPublicReferences = m_publicProjectReferences is { Count: > 0 };

        m_filtersFile = cppProject.Base.OutputDir.MakeSubFileItem(cppProject.Base.Name + VsFileExtension.VcxprojFilters);

        m_projectWriter = new VsXmlWriter(m_project.OutputFile);
    }

    public void Dispose() => m_projectWriter.Dispose();


    private void _GenerateProject()
    {
        // https://docs.microsoft.com/en-us/cpp/build/reference/vcxproj-file-structure?view=msvc-170

        _GenerateTargetPrivatePropsFile();
        _GenerateTargetPublicPropsFile();

        m_projectWriter.BeginProject(null, k_ToolsVersion, k_DefaultTargets, true);

        _ProjectConfigurationsItemGroup();
        _GlobalsPropertyGroup();

        m_projectWriter.Import(VcxprojImports.MicrosoftCppDefaultProps);

        _ConfigurationPropertyGroup();

        m_projectWriter.Import(VcxprojImports.MicrosoftCppProps);

        _ExtensionSettingsImportGroup();
        _SharedImportGroup();
        _PropertySheetsImportGroup();
        _UserMacrosPropertyGroup();

        _PerConfigurationPropertyGroup();
        _PerConfigurationItemDefinitionGroup();

        _SourcesItemGroups();

        m_projectWriter.Import(VcxprojImports.MicrosoftCppTargets);

        _ProjectReferencesItemGroup();
        _ExtensionTargetsImportGroup();

        m_projectWriter.EndElement();
    }

    private void _GenerateTargetPrivatePropsFile()
    {
        if (m_project.PrivateProps is null)
        {
            return;
        }

        using VsXmlWriter propsFileWriter = new(m_project.PrivateProps.File);

        propsFileWriter.BeginProject(null, null, null, true);

        //- [PropertyGroup] Props file import condition variable
        propsFileWriter.BeginPropertyGroup();
        propsFileWriter.Property(m_project.PrivateProps.ImportConditionVariable, VsXmlPropertyValue.True);
        propsFileWriter.EndElement();

        //- [ItemDefinitionGroup] Private system libs
        foreach (CppProjectConfiguration projectConfiguration in m_project.Base.Configurations)
        {
            List<string> privateSystemLibs = projectConfiguration.PrivateSystemLibs;

            if (projectConfiguration.OutputType == OutputType.StaticLib && privateSystemLibs.Count > 0)
            {
                propsFileWriter.BeginItemDefinitionGroup();

                propsFileWriter.BeginItem(VcxprojItem.Link);
                propsFileWriter.Property(VcxprojLinkItem.AdditionalDependencies, privateSystemLibs);
                propsFileWriter.EndElement();

                propsFileWriter.EndElement();
            }
        }

        propsFileWriter.EndElement(); // BeginProject
    }

    private void _GenerateTargetPublicPropsFile()
    {
        if (m_project.PublicProps is null)
        {
            return;
        }

        using VsXmlWriter propsFileWriter = new(m_project.PublicProps.File);

        propsFileWriter.BeginProject(null, null, null, true);

        //- [ImportGroup] Public Conan libs and project references
        foreach (VsCppProjectConfiguration projectConfiguration in m_project.Configurations)
        {
            if (projectConfiguration.Base.OutputType != OutputType.StaticLib)
            {
                continue;
            }

            List<ConanLibrary> publicConanLibs = projectConfiguration.Base.PublicConanLibs;

            if ((publicConanLibs.Count > 0 || m_hasPublicReferences) == false)
            {
                continue;
            }

            propsFileWriter.BeginImportGroup(VcxprojLabel.PropertySheets, projectConfiguration.Condition);

            if (m_hasPublicReferences)
            {
                // NOTE(v.matushkin): Not null when m_hasPublicReferences is true
                foreach (VsCppProject projectReference in m_publicProjectReferences!)
                {
                    VsPropsFile? privateProps = projectReference.PrivateProps;
                    VsPropsFile? publicProps = projectReference.PublicProps;
                    if (privateProps is not null)
                    {
                        propsFileWriter.Import(privateProps.File.GetRelativePath(m_project.Base.OutputDir), privateProps.ImportCondition);
                    }
                    if (publicProps is not null)
                    {
                        propsFileWriter.Import(publicProps.File.GetRelativePath(m_project.Base.OutputDir), publicProps.ImportCondition);
                    }
                }
            }

            foreach (ConanLibrary library in publicConanLibs)
            {
                var propsImport = VsGenerationContext.GetConanLibPropsImportInfo(library);
                propsFileWriter.Import(propsImport.project, propsImport.condition);
            }

            propsFileWriter.EndElement();
        }

        //- [PropertyGroup] Props file import condition variable
        propsFileWriter.BeginPropertyGroup();
        propsFileWriter.Property(m_project.PublicProps.ImportConditionVariable, VsXmlPropertyValue.True);
        propsFileWriter.EndElement();

        //- [ItemDefinitionGroup] Public defines
        foreach (VsCppProjectConfiguration projectConfiguration in m_project.Configurations)
        {
            List<string> publicDefines = projectConfiguration.Base.PublicDefines;

            if (projectConfiguration.Base.OutputType == OutputType.StaticLib && publicDefines.Count > 0)
            {
                propsFileWriter.BeginItemDefinitionGroup(projectConfiguration.Condition);

                propsFileWriter.BeginItem(VcxprojItem.ClCompile);
                propsFileWriter.Property(VcxprojClCompileItem.PreprocessorDefinitions, publicDefines);
                propsFileWriter.EndElement();

                propsFileWriter.EndElement();
            }
        }

        propsFileWriter.EndElement(); // BeginProject
    }

    private void _GenerateFiltersFile()
    {
        HashSet<string> filterDirs = new();

        using VsXmlWriter filtersWriter = new(m_filtersFile);

        filtersWriter.BeginProject(null, null, null, true);

        //- [ItemGroup] ClInclude
        if (m_project.Base.PrivateHeaders.Count > 0 || m_project.Base.PublicHeaders.Count > 0)
        {
            filtersWriter.BeginItemGroup();
            WriteSourceFiles(VcxprojItem.ClInclude, m_project.Base.PrivateHeaders);
            WriteSourceFiles(VcxprojItem.ClInclude, m_project.Base.PublicHeaders);
            filtersWriter.EndElement();
        }

        //- [ItemGroup] ClCompile
        if (m_project.Base.PrivateModules.Count > 0 || m_project.Base.PublicModules.Count > 0 || m_project.Base.Sources.Count > 0)
        {
            filtersWriter.BeginItemGroup();
            WriteSourceFiles(VcxprojItem.ClCompile, m_project.Base.PrivateModules);
            WriteSourceFiles(VcxprojItem.ClCompile, m_project.Base.PublicModules);
            WriteSourceFiles(VcxprojItem.ClCompile, m_project.Base.Sources);
            filtersWriter.EndElement();
        }

        //- [ItemGroup] Filters
        if (filterDirs.Count > 0)
        {
            string[] sortedFilterDirs = filterDirs.ToArray();
            Array.Sort(sortedFilterDirs);

            filtersWriter.BeginItemGroup();
            foreach (string filterDir in sortedFilterDirs)
            {
                filtersWriter.BeginItem("Filter", filterDir);
                filtersWriter.Property("UniqueIdentifier", VisualStudioGuid.Generate());
                filtersWriter.EndElement();
            }
            filtersWriter.EndElement();
        }

        filtersWriter.EndElement(); // BeginProject

        void AddFilterDir(string filterDir)
        {
            if (filterDirs.Add(filterDir))
            {
                for (string? parentFilterDir = PathExtensions.GetParentDir(filterDir); parentFilterDir is not null;)
                {
                    if (filterDirs.Add(parentFilterDir) == false)
                    {
                        break;
                    }

                    parentFilterDir = PathExtensions.GetParentDir(parentFilterDir);
                }
            }
        }

        void WriteSourceFiles(string tag, ReadOnlyCollection<FileItem> sourceFiles)
        {
            foreach (FileItem sourceFile in sourceFiles)
            {
                string filterDir = Path.GetDirectoryName(Path.GetRelativePath(m_project.Base.MakeFileDir.FullPath, sourceFile.FullPath))!;
                AddFilterDir(filterDir);

                filtersWriter.BeginItem(tag, sourceFile.GetRelativePath(m_project.Base.OutputDir));
                filtersWriter.Property("Filter", filterDir);
                filtersWriter.EndElement();
            }
        }
    }


    #region _GenerateProject methods

    private void _ProjectConfigurationsItemGroup()
    {
        m_projectWriter.BeginItemGroup(VcxprojLabel.ProjectConfigurations);

        foreach (VsCppProjectConfiguration projectConfiguration in m_project.Configurations)
        {
            m_projectWriter.BeginItem(VcxprojProperty.ProjectConfiguration, projectConfiguration.ConfigurationPlatform);
            m_projectWriter.Property(VcxprojProperty.Configuration, projectConfiguration.Base.Name);
            m_projectWriter.Property(VcxprojProperty.Platform, projectConfiguration.Base.PlatformString);
            m_projectWriter.EndElement();
        }

        m_projectWriter.EndElement();
    }

    private void _GlobalsPropertyGroup()
    {
        m_projectWriter.BeginPropertyGroup(VcxprojLabel.Globals);

        m_projectWriter.ProjectGuid(m_project.ProjectGuid);
        m_projectWriter.Property(VcxprojGlobalsPropertyName.Keyword, VcxprojGlobalsPropertyValue.Keyword);
        m_projectWriter.Property(VcxprojGlobalsPropertyName.WindowsTargetPlatformVersion, VcxprojGlobalsPropertyValue.WindowsTargetPlatformVersion);

        m_projectWriter.EndElement();
    }

    private void _ConfigurationPropertyGroup()
    {
        foreach (VsCppProjectConfiguration projectConfiguration in m_project.Configurations)
        {
            m_projectWriter.BeginPropertyGroup(VcxprojLabel.Configuration, projectConfiguration.Condition);

            m_projectWriter.Property(VcxprojProperty.ConfigurationType, _OutputTypeToConfigurationType(projectConfiguration.Base.OutputType));

            _BuildSystemConfigurationPropertyGroup((MSBuildOptions)projectConfiguration.Base.BuildSystemOptions);

            m_projectWriter.EndElement();
        }
    }

    private void _ExtensionSettingsImportGroup()
    {
        m_projectWriter.BeginImportGroup(VcxprojLabel.ExtensionSettings);
        m_projectWriter.EndElement();
    }

    private void _SharedImportGroup()
    {
        m_projectWriter.BeginImportGroup(VcxprojLabel.Shared);
        m_projectWriter.EndElement();
    }

    private void _PropertySheetsImportGroup()
    {
        DirectoryItem projectOutputDir = m_project.Base.OutputDir;

        foreach (VsCppProjectConfiguration projectConfiguration in m_project.Configurations)
        {
            m_projectWriter.BeginImportGroup(VcxprojLabel.PropertySheets, projectConfiguration.Condition);

            // Order of imports matters!

            m_projectWriter.Import(
                VcxprojImports.MicrosoftCppPlatformUserProps,
                @"exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')",
                "LocalAppDataPlatform");

            foreach (ConanLibrary library in projectConfiguration.Base.PrivateConanLibs)
            {
                var propsImport = VsGenerationContext.GetConanLibPropsImportInfo(library);
                m_projectWriter.Import(propsImport.project, propsImport.condition);
            }

            if (m_project.PublicProps is not null)
            {
                m_projectWriter.Import(m_project.PublicProps.File.GetRelativePath(projectOutputDir));
            }

            if (m_hasPrivateReferences)
            {
                foreach (VsCppProject projectReference in m_privateProjectReferences!) // NOTE(v.matushkin): Not null when hasReferences is true
                {
                    VsPropsFile? privateProps = projectReference.PrivateProps;
                    VsPropsFile? publicProps = projectReference.PublicProps;
                    if (privateProps is not null)
                    {
                        m_projectWriter.Import(privateProps.File.GetRelativePath(projectOutputDir), privateProps.ImportCondition);
                    }
                    if (publicProps is not null)
                    {
                        m_projectWriter.Import(publicProps.File.GetRelativePath(projectOutputDir), publicProps.ImportCondition);
                    }
                }
            }

            if (m_project.Base.BaseGeneratedIncludeDir is not null)
            {
                // TODO(v.matushkin): Shouldn't be hardcoded?
                m_projectWriter.Import(HeaderToolTask.PropsFileName);
            }

            m_projectWriter.EndElement(); // BeginImportGroup
        }
    }

    private void _UserMacrosPropertyGroup()
    {
        m_projectWriter.BeginPropertyGroup(VcxprojLabel.UserMacros);
        m_projectWriter.EndElement();
    }

    private void _PerConfigurationPropertyGroup()
    {
        foreach (VsCppProjectConfiguration projectConfiguration in m_project.Configurations)
        {
            CppProjectConfiguration cppProjectConfiguration = projectConfiguration.Base;

            m_projectWriter.BeginPropertyGroup(null, projectConfiguration.Condition);

            _BuildSystemPerConfigurationPropertyGroup((MSBuildOptions)cppProjectConfiguration.BuildSystemOptions);

            m_projectWriter.Property(VcxprojProperty.OutDir, cppProjectConfiguration.OutputDir);
            m_projectWriter.Property(VcxprojProperty.IntDir, cppProjectConfiguration.IntermediateDir);

            if (cppProjectConfiguration.OutputType == OutputType.StaticLib)
            {
                m_projectWriter.Property(VcxprojProperty.AllProjectIncludesArePublic, VsXmlPropertyValue.False);
                m_projectWriter.Property(VcxprojProperty.AllProjectBMIsArePublic, VsXmlPropertyValue.False);

                m_projectWriter.Property(VcxprojProperty.PublicIncludeDirectories, m_project.Base.PublicIncludeDirs);
                m_projectWriter.Property(VcxprojProperty.PublicModuleDirectories, m_project.Base.PublicModuleDirs);
            }

            m_projectWriter.EndElement();
        }
    }

    private void _PerConfigurationItemDefinitionGroup()
    {
        foreach (VsCppProjectConfiguration projectConfiguration in m_project.Configurations)
        {
            CppProjectConfiguration cppProjectConfiguration = projectConfiguration.Base;

            m_projectWriter.BeginItemDefinitionGroup(projectConfiguration.Condition);

            //- [Item] CopiumHeaderTool
            if (m_project.Base.BaseGeneratedIncludeDir is not null)
            {
                m_projectWriter.BeginItem(VcxprojItem.CopiumHeaderTool);
                m_projectWriter.Property(VcxprojCopiumHeaderToolProperty.BaseGeneratedIncludeDir, m_project.Base.BaseGeneratedIncludeDir);
                m_projectWriter.EndElement();
            }

            //- [Item] ClCompile
            m_projectWriter.BeginItem(VcxprojItem.ClCompile);
            m_projectWriter.Property(VcxprojClCompileItem.AdditionalIncludeDirectories, m_project.Base.IncludeDirs);
            m_projectWriter.Property(VcxprojClCompileItem.AdditionalOptions, cppProjectConfiguration.AdditionalCompilerOptions, " ");
            _WriteOptions(cppProjectConfiguration.CompilerOptions);
            m_projectWriter.EndElement();

            //- [Item] Lib or Link
            m_projectWriter.BeginItem(cppProjectConfiguration.OutputType == OutputType.StaticLib ? VcxprojItem.Lib : VcxprojItem.Link);
            _WriteOptions(cppProjectConfiguration.LinkerOptions);
            m_projectWriter.EndElement();

            m_projectWriter.EndElement(); // BeginItemDefinitionGroup
        }
    }

    private void _SourcesItemGroups()
    {
        CppProject cppProject = m_project.Base;

        //- [ItemGroup] ClInclude
        if (cppProject.PrivateHeaders.Count > 0 || cppProject.PublicHeaders.Count > 0)
        {
            m_projectWriter.BeginItemGroup();
            WriteSourceFiles(VcxprojItem.ClInclude, cppProject.PrivateHeaders);
            WriteSourceFiles(VcxprojItem.ClInclude, cppProject.PublicHeaders);
            m_projectWriter.EndElement();
        }

        //- [ItemGroup] ClCompile
        if (cppProject.PrivateModules.Count > 0 || cppProject.PublicModules.Count > 0 || cppProject.Sources.Count > 0)
        {
            m_projectWriter.BeginItemGroup();
            WriteSourceFiles(VcxprojItem.ClCompile, cppProject.PrivateModules);
            WriteSourceFiles(VcxprojItem.ClCompile, cppProject.PublicModules);
            WriteSourceFiles(VcxprojItem.ClCompile, cppProject.Sources);
            m_projectWriter.EndElement();
        }

        void WriteSourceFiles(string itemName, ReadOnlyCollection<FileItem> sourceFiles)
        {
            foreach (FileItem sourceFile in sourceFiles)
            {
                m_projectWriter.Item(itemName, sourceFile.GetRelativePath(cppProject.OutputDir));
            }
        }
    }

    private void _ProjectReferencesItemGroup()
    {
        if ((m_hasPrivateReferences || m_hasPublicReferences) == false)
        {
            return;
        }

        // NOTE(v.matushkin): If I get it right, vcxproj ProjectReference is not transitive,
        //  so I need to add them all manually
        IEnumerable<VsCppProject> directProjectReferences = Enumerable.Empty<VsCppProject>();
        // NOTE(v.matushkin): Not null when hasReferences is true
        if (m_hasPrivateReferences) directProjectReferences = m_privateProjectReferences!;
        if (m_hasPublicReferences) directProjectReferences = directProjectReferences.Concat(m_publicProjectReferences!);

        m_projectWriter.BeginItemGroup();

        foreach (VsCppProject projectReference in _GetAllProjectReferences(directProjectReferences))
        {
            m_projectWriter.BeginItem(VsXmlItem.ProjectReference, projectReference.OutputFile.GetRelativePath(m_project.Base.OutputDir));
            m_projectWriter.Property(VcxprojItem.Project, projectReference.ProjectGuid);
            m_projectWriter.EndElement();
        }

        m_projectWriter.EndElement();
    }

    private void _ExtensionTargetsImportGroup()
    {
        m_projectWriter.BeginImportGroup(VcxprojLabel.ExtensionTargets);

        if (m_project.Base.BaseGeneratedIncludeDir is not null)
        {
            // TODO(v.matushkin): Shouldn't be hardcoded? Or should be? May be through config?
            m_projectWriter.Import(HeaderToolTask.TargetFileName);
        }

        m_projectWriter.EndElement();
    }

    #endregion


    private static string _OutputTypeToConfigurationType(OutputType outputType)
    {
        return outputType switch
        {
            OutputType.Executable => "Application",
            OutputType.StaticLib => "StaticLibrary",
            OutputType.SharedLib => "DynamicLibrary",
            _ => throw new ArgumentOutOfRangeException(nameof(outputType)),
        };
    }

    private static VsCppProject[] _GetAllProjectReferences(IEnumerable<VsCppProject> directProjectReferences)
    {
        HashSet<VsCppProject> allProjectReferences = new();

        foreach (VsCppProject directReference in directProjectReferences)
        {
            if (allProjectReferences.Add(directReference))
            {
                AddTransitiveReferences(directReference);
            }
        }

        VsCppProject[] arr = allProjectReferences.ToArray();
        Array.Sort(arr);

        return arr;

        void AddTransitiveReferences(VsCppProject projectReference)
        {
            if (projectReference.PublicProjectReferences is not null)
            {
                foreach (VsCppProject transitiveReference in projectReference.PublicProjectReferences)
                {
                    if (allProjectReferences.Add(transitiveReference))
                    {
                        AddTransitiveReferences(transitiveReference);
                    }
                }
            }
            if (projectReference.PrivateProjectReferences is not null)
            {
                foreach (VsCppProject transitiveReference in projectReference.PrivateProjectReferences)
                {
                    if (allProjectReferences.Add(transitiveReference))
                    {
                        AddTransitiveReferences(transitiveReference);
                    }
                }
            }
        }
    }
}
