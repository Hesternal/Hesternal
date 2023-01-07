namespace Hesternal.BuildTool.Generators.VisualStudio;


internal static class VsXmlItem
{
    public const string Import              = "Import";
    public const string ImportGroup         = "ImportGroup";
    public const string ItemDefinitionGroup = "ItemDefinitionGroup";
    public const string ItemGroup           = "ItemGroup";
    public const string Project             = "Project";
    public const string ProjectGuid         = "ProjectGuid";
    public const string ProjectReference    = "ProjectReference";
    public const string PropertyGroup       = "PropertyGroup";
}

internal static class VsXmlAttribute
{
    public const string Condition      = "Condition";
    public const string DefaultTargets = "DefaultTargets";
    public const string Include        = "Include";
    public const string Label          = "Label";
    public const string Project        = "Project";
    public const string Sdk            = "Sdk";
    public const string ToolsVersion   = "ToolsVersion";
}

internal static class VsXmlPropertyValue
{
    public const string False = "False";
    public const string True  = "True";
}


internal static class CsprojItem
{
    public const string Compile = "Compile";
}

internal static class CsprojProjectReferenceProperty
{
    public const string Private = "Private";
}


internal static class VcxprojItem
{
    public const string ClCompile           = "ClCompile";
    public const string ClInclude           = "ClInclude";
    public const string HesternalHeaderTool = "HesternalHeaderTool";
    public const string Lib                 = "Lib";
    public const string Link                = "Link";
    public const string Project             = "Project";
}

internal static class VcxprojClCompileItem
{
    public const string AdditionalIncludeDirectories = "AdditionalIncludeDirectories";
    public const string AdditionalOptions            = "AdditionalOptions";
    public const string PreprocessorDefinitions      = "PreprocessorDefinitions";
}

internal static class VcxprojLabel
{
    public const string Configuration         = "Configuration";
    public const string ExtensionSettings     = "ExtensionSettings";
    public const string ExtensionTargets      = "ExtensionTargets";
    public const string Globals               = "Globals";
    public const string ProjectConfigurations = "ProjectConfigurations";
    public const string PropertySheets        = "PropertySheets";
    public const string Shared                = "Shared";
    public const string UserMacros            = "UserMacros";
}

internal static class VcxprojProperty
{
    // ProjectConfigurations
    public const string ProjectConfiguration        = "ProjectConfiguration";
    public const string Configuration               = "Configuration";
    public const string Platform                    = "Platform";
    // Configuration
    public const string ConfigurationType           = "ConfigurationType";
    // General
    public const string OutDir                      = "OutDir";
    public const string IntDir                      = "IntDir";
    // VC++ Directories
    public const string AllProjectIncludesArePublic = "AllProjectIncludesArePublic";
    public const string AllProjectBMIsArePublic     = "AllProjectBMIsArePublic";
    public const string PublicIncludeDirectories    = "PublicIncludeDirectories";
    public const string PublicModuleDirectories     = "PublicModuleDirectories";
}

internal static class VcxprojLinkItem
{
    public const string AdditionalDependencies = "AdditionalDependencies";
}

internal static class VcxprojHesternalHeaderToolProperty
{
    public const string BaseGeneratedIncludeDir = "BaseGeneratedIncludeDir";
}
