using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using Copium.BuildTool.Build;
using Copium.BuildTool.BuildRules;
using Copium.BuildTool.Core;

namespace Copium.BuildTool.Project.VisualStudio;


internal sealed partial class VsCppProjectGenerator : IDisposable
{
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


    private readonly VsGenerationContext m_generationContext;
    private readonly CppTarget m_cppTarget;
    private readonly VsCppProjectFile? m_referenceProjectFile;

    private readonly VsCppProjectFile m_projectFile;
    private DirectoryItem ProjectDir => m_projectFile.ProjectDir;
    private readonly FileItem m_filtersFile;

    private readonly VsXmlWriter m_projectWriter;


    public static void Generate(VsGenerationContext generationContext, CppTarget cppTarget)
    {
        using VsCppProjectGenerator generator = new(generationContext, cppTarget);
        generator._GenerateProject();
        generator._GenerateFiltersFile();
    }


    private VsCppProjectGenerator(VsGenerationContext generationContext, CppTarget cppTarget)
    {
        m_generationContext = generationContext;
        m_cppTarget = cppTarget;
        if (cppTarget.TargetReference != null)
        {
            m_referenceProjectFile = generationContext.GetCppProject(cppTarget.TargetReference);
        }

        m_projectFile = generationContext.GetCppProject(cppTarget.TargetName);
        m_filtersFile = m_projectFile.ProjectDir.MakeSubFileItem(cppTarget.TargetName + VsFileExtension.VcxprojFilters);

        m_projectWriter = new VsXmlWriter(m_projectFile);
    }

    public void Dispose() => m_projectWriter.Dispose();


    private void _GenerateProject()
    {
        if (m_cppTarget.eTargetType == TargetType.Engine)
        {
            _GenerateTargetPrivatePropsFile();
            _GenerateTargetPublicPropsFile();
        }

        m_projectWriter.BeginProject(null, VisualStudioConfig.Vcxproj.ToolsVersion, VisualStudioConfig.Vcxproj.DefaultTargets, true);

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
        using VsXmlWriter propsFileWriter = new(m_projectFile.PropsPrivateFile!);

        propsFileWriter.BeginProject(null, null, null, true);

        if (m_cppTarget.PrivateSystemLibs.Any())
        {
            propsFileWriter.BeginItemDefinitionGroup();

            propsFileWriter.BeginItem(VcxprojItem.Link);
            propsFileWriter.Property(VcxprojLinkItem.AdditionalDependencies, m_cppTarget.PrivateSystemLibs);
            propsFileWriter.EndElement();

            propsFileWriter.EndElement();
        }

        propsFileWriter.EndElement(); // BeginProject
    }

    private void _GenerateTargetPublicPropsFile()
    {
        using VsXmlWriter propsFileWriter = new(m_projectFile.PropsPublicFile!);

        propsFileWriter.BeginProject(null, null, null, true);

        propsFileWriter.BeginImportGroup(VcxprojLabel.PropertySheets);
        foreach (ConanLibrary library in m_cppTarget.PublicConanLibs)
        {
            var propsImport = VsGenerationContext.GetConanLibPropsImportInfo(library);
            propsFileWriter.Import(propsImport.project, null, propsImport.condition);
        }
        propsFileWriter.EndElement();

        void WritePreprocessorDefinitions(List<string> defines)
        {
            propsFileWriter.BeginItem(VcxprojItem.ClCompile);
            propsFileWriter.Property(VcxprojClCompileItem.PreprocessorDefinitions, defines);
            propsFileWriter.EndElement();
        }

        if (m_cppTarget.PublicDefines.Common.Count != 0)
        {
            propsFileWriter.BeginItemDefinitionGroup();
            WritePreprocessorDefinitions(m_cppTarget.PublicDefines.Common);
            propsFileWriter.EndElement();
        }

        foreach (VsCppProjectConfiguration projectConfiguration in m_generationContext.CppProjectConfigurations)
        {
            List<string> targetPublicDefines = projectConfiguration.Configuration switch
            {
                BuildConfiguration.Debug       => m_cppTarget.PublicDefines.Debug,
                BuildConfiguration.Development => m_cppTarget.PublicDefines.Development,
                BuildConfiguration.Release     => m_cppTarget.PublicDefines.Release,
                _ => throw new ArgumentOutOfRangeException(),
            };

            if (targetPublicDefines.Count != 0)
            {
                propsFileWriter.BeginItemDefinitionGroup(projectConfiguration.Condition);
                WritePreprocessorDefinitions(targetPublicDefines);
                propsFileWriter.EndElement();
            }
        }

        propsFileWriter.EndElement(); // BeginProject
    }

    private void _GenerateFiltersFile()
    {
        HashSet<string> filterDirs = new();

        using VsXmlWriter filtersWriter = new(m_filtersFile);

        void AddFilterDir(string filterDir)
        {
            if (filterDirs.Add(filterDir))
            {
                for (string? parentFilterDir = PathExtensions.GetParentDir(filterDir); parentFilterDir != null;)
                {
                    if (filterDirs.Add(parentFilterDir) == false)
                    {
                        break;
                    }

                    parentFilterDir = PathExtensions.GetParentDir(parentFilterDir);
                }
            }
        }

        void WriteSourceFiles(string tag, FileItem[]? sourceFiles)
        {
            if (sourceFiles == null)
            {
                return;
            }
            foreach (FileItem sourceFile in sourceFiles)
            {
                string filterDir = Path.GetDirectoryName(Path.GetRelativePath(m_cppTarget.TargetDirPath, sourceFile.FullPath))!;
                AddFilterDir(filterDir);

                filtersWriter.BeginItem(tag, sourceFile.GetRelativePath(ProjectDir));
                filtersWriter.Property("Filter", filterDir);
                filtersWriter.EndElement();
            }
        }

        filtersWriter.BeginProject(null, null, null, true);

        //- Sources ClCompile
        filtersWriter.BeginItemGroup();
        foreach (Module module in m_cppTarget.Modules)
        {
            WriteSourceFiles(VcxprojItem.ClCompile, module.PrivateModules);
            WriteSourceFiles(VcxprojItem.ClCompile, module.PublicModules);
            WriteSourceFiles(VcxprojItem.ClCompile, module.Sources);
        }
        filtersWriter.EndElement();

        //- Headers ClInclude
        filtersWriter.BeginItemGroup();
        foreach (Module module in m_cppTarget.Modules)
        {
            WriteSourceFiles(VcxprojItem.ClInclude, module.PrivateHeaders);
            WriteSourceFiles(VcxprojItem.ClInclude, module.PublicHeaders);
        }
        filtersWriter.EndElement();

        //- Filters
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

        filtersWriter.EndElement();
    }


    #region _GenerateProject methods

    private void _ProjectConfigurationsItemGroup()
    {
        m_projectWriter.BeginItemGroup(VcxprojLabel.ProjectConfigurations);

        foreach (VsCppProjectConfiguration projectConfiguration in m_generationContext.CppProjectConfigurations)
        {
            m_projectWriter.BeginItem(VcxprojProperty.ProjectConfiguration, projectConfiguration.ConfigurationPlatform);
            m_projectWriter.Property(VcxprojProperty.Configuration, projectConfiguration.ConfigurationName);
            m_projectWriter.Property(VcxprojProperty.Platform, projectConfiguration.PlatformName);
            m_projectWriter.EndElement();
        }

        m_projectWriter.EndElement();
    }

    private void _GlobalsPropertyGroup()
    {
        m_projectWriter.BeginPropertyGroup(VcxprojLabel.Globals);

        m_projectWriter.ProjectGuid(m_projectFile.ProjectGuid);
        foreach (NameValue property in VisualStudioConfig.Vcxproj.Globals)
        {
            m_projectWriter.Property(property.Name, property.Value);
        }

        m_projectWriter.EndElement();
    }

    private void _ConfigurationPropertyGroup()
    {
        foreach (VsCppProjectConfiguration projectConfiguration in m_generationContext.CppProjectConfigurations)
        {
            m_projectWriter.BeginPropertyGroup(VcxprojLabel.Configuration, projectConfiguration.Condition);

            m_projectWriter.Property(VcxprojProperty.ConfigurationType, _TargetTypeToConfigurationType(m_cppTarget.eTargetType));
            foreach (NameValue property in VisualStudioConfig.Vcxproj.DefaultProps)
            {
                m_projectWriter.Property(property.Name, property.Value);
            }

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
        m_projectWriter.BeginImportGroup(VcxprojLabel.PropertySheets);

        // Order of imports matters!
        // NOTE(v.matushkin): CMake adds only one PropertySheet ImportGroup,
        //  VisualStudio adds it for every Configuration|Platform combo

        m_projectWriter.Import(
            VcxprojImports.MicrosoftCppPlatformUserProps,
            "LocalAppDataPlatform",
            @"exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')");

        foreach (ConanLibrary library in m_cppTarget.PrivateConanLibs)
        {
            var propsImport = VsGenerationContext.GetConanLibPropsImportInfo(library);
            m_projectWriter.Import(propsImport.project, null, propsImport.condition);
        }

        // TODO(v.matushkin): Shouldn't be hardcoded?
        m_projectWriter.Import(m_generationContext.CopiumCppPropsFile.GetRelativePath(ProjectDir));
        m_projectWriter.Import(m_generationContext.CopiumCppMsvcPropsFile.GetRelativePath(ProjectDir));

        if (m_projectFile.PropsPublicFile != null)
        {
            m_projectWriter.Import(m_projectFile.PropsPublicFile.GetRelativePath(ProjectDir));
        }

        if (m_referenceProjectFile != null)
        {
            if (m_referenceProjectFile.PropsPublicFile != null)
            {
                m_projectWriter.Import(m_referenceProjectFile.PropsPublicFile.GetRelativePath(ProjectDir));
            }
            if (m_referenceProjectFile.PropsPrivateFile != null)
            {
                m_projectWriter.Import(m_referenceProjectFile.PropsPrivateFile.GetRelativePath(ProjectDir));
            }
        }

        // TODO(v.matushkin): Shouldn't be hardcoded?
        m_projectWriter.Import(HeaderToolTask.PropsFileName);

        m_projectWriter.EndElement();
    }

    private void _UserMacrosPropertyGroup()
    {
        m_projectWriter.BeginPropertyGroup(VcxprojLabel.UserMacros);
        m_projectWriter.EndElement();
    }

    private void _PerConfigurationPropertyGroup()
    {
        if (m_cppTarget.eTargetType == TargetType.Engine)
        {
            string? publicIncludeDirectories = m_projectWriter.MakePropertyValue(VcxprojProperty.PublicIncludeDirectories, m_cppTarget.PublicIncludeDirs);
            string? publicModuleDirectories = m_projectWriter.MakePropertyValue(VcxprojProperty.PublicModuleDirectories, m_cppTarget.PublicModuleDirs);

            foreach (VsCppProjectConfiguration projectConfiguration in m_generationContext.CppProjectConfigurations)
            {
                m_projectWriter.BeginPropertyGroup(null, projectConfiguration.Condition);

                m_projectWriter.Property(VcxprojProperty.OutDir, VisualStudioConfig.Vcxproj.OutDir);
                m_projectWriter.Property(VcxprojProperty.IntDir, VisualStudioConfig.Vcxproj.IntDir);

                m_projectWriter.Property(VcxprojProperty.AllProjectIncludesArePublic, VsXmlPropertyValue.False);
                m_projectWriter.Property(VcxprojProperty.AllProjectBMIsArePublic, VsXmlPropertyValue.False);

                if (publicIncludeDirectories != null)
                {
                    m_projectWriter.Property(VcxprojProperty.PublicIncludeDirectories, publicIncludeDirectories);
                }

                if (publicModuleDirectories != null)
                {
                    m_projectWriter.Property(VcxprojProperty.PublicModuleDirectories, publicModuleDirectories);
                }

                m_projectWriter.EndElement();
            }
        }
        else
        {
            foreach (VsCppProjectConfiguration projectConfiguration in m_generationContext.CppProjectConfigurations)
            {
                m_projectWriter.BeginPropertyGroup(null, projectConfiguration.Condition);

                m_projectWriter.Property(VcxprojProperty.OutDir, VisualStudioConfig.Vcxproj.OutDir);
                m_projectWriter.Property(VcxprojProperty.IntDir, VisualStudioConfig.Vcxproj.IntDir);

                m_projectWriter.EndElement();
            }
        }
    }

    private void _PerConfigurationItemDefinitionGroup()
    {
        foreach (VsCppProjectConfiguration projectConfiguration in m_generationContext.CppProjectConfigurations)
        {
            m_projectWriter.BeginItemDefinitionGroup(projectConfiguration.Condition);

            //- CopiumHeaderTool
            m_projectWriter.BeginItem(VcxprojItem.CopiumHeaderTool);
            m_projectWriter.Property(VcxprojCopiumHeaderToolProperty.BaseGeneratedIncludeDir, m_cppTarget.BaseGeneratedIncludeDir);
            m_projectWriter.EndElement();

            //- ClCompile
            m_projectWriter.BeginItem(VcxprojItem.ClCompile);
            m_projectWriter.Property(VcxprojClCompileItem.AdditionalIncludeDirectories, m_cppTarget.IncludeDirs);
            m_projectWriter.EndElement();

            //- Link
            if (m_cppTarget.eTargetType == TargetType.Editor)
            {
                // NOTE(v.matushkin): I should handle this in configuration file just like the other flags,
                //  but I'm not sure if I can do that
                NameValue subsytemFlag = VisualStudioConfig.Msvc.Link["SUBSYSTEM:CONSOLE"];

                m_projectWriter.BeginItem(VcxprojItem.Link);
                m_projectWriter.Property(subsytemFlag.Name, subsytemFlag.Value);
                m_projectWriter.EndElement();
            }

            m_projectWriter.EndElement(); // BeginItemDefinitionGroup
        }
    }

    private void _SourcesItemGroups()
    {
        void AddSourceFiles(string itemTag, FileItem[]? sourceFiles)
{
            if (sourceFiles != null)
            {
                foreach (FileItem sourceFile in sourceFiles)
                {
                    m_projectWriter.Item(itemTag, sourceFile.GetRelativePath(ProjectDir));
                }
            }
        }

        //- ClInclude ItemGroup
        m_projectWriter.BeginItemGroup();
        foreach (Module module in m_cppTarget.Modules)
        {
            AddSourceFiles(VcxprojItem.ClInclude, module.PrivateHeaders);
            AddSourceFiles(VcxprojItem.ClInclude, module.PublicHeaders);
        }
        m_projectWriter.EndElement();

        //- ClCompile ItemGroup
        m_projectWriter.BeginItemGroup();
        foreach (Module module in m_cppTarget.Modules)
        {
            AddSourceFiles(VcxprojItem.ClCompile, module.PrivateModules);
            AddSourceFiles(VcxprojItem.ClCompile, module.PublicModules);
            AddSourceFiles(VcxprojItem.ClCompile, module.Sources);
        }
        m_projectWriter.EndElement();
    }

    private void _ProjectReferencesItemGroup()
    {
        if (m_referenceProjectFile != null)
        {
            m_projectWriter.BeginItemGroup();

            m_projectWriter.BeginItem(VsXmlItem.ProjectReference, m_referenceProjectFile.GetRelativePath(ProjectDir));
            m_projectWriter.Property(VcxprojItem.Project, m_referenceProjectFile.ProjectGuid);
            m_projectWriter.EndElement();

            m_projectWriter.EndElement();
        }
    }

    private void _ExtensionTargetsImportGroup()
    {
        m_projectWriter.BeginImportGroup(VcxprojLabel.ExtensionTargets);

        // TODO(v.matushkin): Shouldn't be hardcoded? Or should be? May be through config?
        m_projectWriter.Import(HeaderToolTask.TargetFileName);

        m_projectWriter.EndElement();
    }

    #endregion


    private static string _TargetTypeToConfigurationType(TargetType type)
    {
        return type == TargetType.Engine ? "StaticLibrary" : "Application";
    }
}
