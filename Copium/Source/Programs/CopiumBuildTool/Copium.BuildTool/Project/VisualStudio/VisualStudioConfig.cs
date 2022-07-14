using System.Collections.Generic;
using System.Text.Json.Serialization;

using Copium.BuildTool.Core;

namespace Copium.BuildTool.Project.VisualStudio;


internal readonly struct NameValue
{
    public readonly string Name;
    public readonly string Value;


    [JsonConstructor]
    public NameValue(string name, string value)
    {
        Name = name;
        Value = value;
    }
}

internal sealed class CsprojConfig
{
    public readonly string Sdk;
    public readonly string ToolsVersion;
    public readonly string DefaultTargets;
    public readonly NameValue[] Properties;


    public CsprojConfig(string sdk, string toolsVersion, string defaultTargets, NameValue[] properties)
    {
        Sdk = sdk;
        ToolsVersion = toolsVersion;
        DefaultTargets = defaultTargets;
        Properties = properties;
    }
}

internal sealed class VcxprojConfig
{
    public readonly string ToolsVersion;
    public readonly string DefaultTargets;
    public readonly string OutDir;
    public readonly string IntDir;
    public readonly NameValue[] Globals;
    public readonly NameValue[] DefaultProps;


    public VcxprojConfig(
        string toolsVersion, string defaultTargets,
        string outDir, string intDir,
        NameValue[] globals, NameValue[] defaultProps
        )
    {
        ToolsVersion = toolsVersion;
        DefaultTargets = defaultTargets;
        OutDir = outDir;
        IntDir = intDir;
        Globals = globals;
        DefaultProps = defaultProps;
    }
}

internal sealed class VcxprojCompilerFlagsTable
{
    public readonly Dictionary<string, NameValue> Cpp;
    public readonly Dictionary<string, NameValue> Link;


    public VcxprojCompilerFlagsTable(Dictionary<string, NameValue> cpp, Dictionary<string, NameValue> link)
    {
        Cpp = cpp;
        Link = link;
    }
}


internal static class VisualStudioConfig
{
    public static readonly CsprojConfig Csproj;
    public static readonly VcxprojConfig Vcxproj;
    public static readonly VcxprojCompilerFlagsTable Msvc;


    static VisualStudioConfig()
    {
        DirectoryItem copiumBuildToolConfigDir = CopiumGlobal.Copium.ConfigDir.MakeSubDirItem("CopiumBuildTool");

        Csproj = copiumBuildToolConfigDir.MakeSubFileItem("Csproj.json").JsonDeserialize<CsprojConfig>();
        Vcxproj = copiumBuildToolConfigDir.MakeSubFileItem("Vcxproj.json").JsonDeserialize<VcxprojConfig>();
        Msvc = copiumBuildToolConfigDir.MakeSubFileItem("MSVC.json").JsonDeserialize<VcxprojCompilerFlagsTable>();
    }
}
