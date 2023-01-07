using System;

namespace Copium.BuildTool.Generators.VisualStudio;


internal static class VisualStudioGuid
{
    private const string k_GuidFormat = "B";

    /// <summary>
    /// .csproj GUID for the new type of .Net Core/.Net Sdk style projects
    /// </summary>
    public static readonly string Csproj  = "{9A19103F-16F7-4668-BE54-9A1E7A4F7556}";
    public static readonly string Vcxproj = "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";

    public static readonly string SolutionFolder = "{2150E333-8FDC-42A3-9474-1A3956D46DE8}";


    public static string Generate() => Guid.NewGuid().ToString(k_GuidFormat).ToUpper();

    public static bool Validate(string guid) => Guid.TryParseExact(guid, k_GuidFormat, out Guid _);
}
