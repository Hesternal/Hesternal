using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;

using Copium.Core.Net7;

using Microsoft.Extensions.Logging;

namespace Copium.BuildTool.External;


internal sealed class DotnetHelper
{
    private static class DotnetStartInfo
    {
        private const string k_FileName = "dotnet";
        private const string k_OptionListRuntimes = "--list-runtimes";

        public static ProcessStartInfo ListSdks()
        {
            ProcessStartInfo dotnetStartIfno = new(k_FileName, k_OptionListRuntimes);
            dotnetStartIfno.UseShellExecute = false;
            dotnetStartIfno.RedirectStandardOutput = true;
            dotnetStartIfno.CreateNoWindow = true;

            return dotnetStartIfno;
        }
    }


    private const string k_DotnetRuntimePackageName = "Microsoft.NETCore.App";
    private const string k_DotnetRefPackageName = k_DotnetRuntimePackageName + ".Ref";
    private const int k_DotnetRequierdMajorVersion = 7;

    private static DotnetHelper? s_Instance;
    public static DotnetHelper Instance => s_Instance ??= new();


    public readonly DirectoryItem RefAssembliesDir;


    private DotnetHelper()
    {
        (DirectoryItem dotnetDir, string dotnetRuntimeVersionString) = _GetDotnetInfo();

        Log.Logger.LogInformation("Found dotnet {version} [{path}]", dotnetRuntimeVersionString, dotnetDir.FullPath);

        RefAssembliesDir = dotnetDir.MakeSubDirItem(
            Path.Join("packs", k_DotnetRefPackageName),
            dotnetRuntimeVersionString,
            Path.Join("ref", "net7.0")
        );
        if (RefAssembliesDir.Exists == false)
        {
            throw new CbtException("Couldn't find dotnet ref assemblies dir");
        }
    }


    private static (DirectoryItem dotnetDir, string dotnetRuntimeVersionString) _GetDotnetInfo()
    {
        List<string> sdkInfos = new();

        using (Process dotnetProcess = new())
        {
            dotnetProcess.StartInfo = DotnetStartInfo.ListSdks();
            dotnetProcess.Start();

            StreamReader outputReader = dotnetProcess.StandardOutput;

            string? line;
            while ((line = outputReader.ReadLine()) != null)
            {
                sdkInfos.Add(line);
            }

            dotnetProcess.WaitForExit();
        }

        const int fullVersionGroup = 1;
        const int versionGroup = 1 + fullVersionGroup;
        const int optionalPreviewGroup = 1 + versionGroup;
        const int pathGroup = 1 + optionalPreviewGroup;
        Regex sdkInfoRegex = new(
            @"Microsoft\.NETCore\.App (([0-9\.]+)(?:\-[a-z]+\.([0-9\.]+))) \[([^\]]+)\]",
            RegexOptions.Compiled | RegexOptions.NonBacktracking);

        Version? runtimeVersion = null;
        Version? runtimePreviewVersion = null;
        string? runtimePath = null;
        string fullVersionString = string.Empty;

        foreach (string sdkInfo in sdkInfos)
        {
            Match sdkInfoMatch = sdkInfoRegex.Match(sdkInfo);
            if (sdkInfoMatch.Success == false)
            {
                continue;
            }

            fullVersionString = sdkInfoMatch.Groups[fullVersionGroup].Value;
            string runtimeVersionString = sdkInfoMatch.Groups[versionGroup].Value;

            Version version = Version.Parse(runtimeVersionString);
            if (version.Major == k_DotnetRequierdMajorVersion)
            {
                Group previewVersionGroup = sdkInfoMatch.Groups[optionalPreviewGroup];

                if (runtimeVersion == null || version > runtimeVersion)
                {
                    runtimeVersion = version;
                    runtimePath = sdkInfoMatch.Groups[pathGroup].Value;

                    if (previewVersionGroup.Success)
                    {
                        runtimePreviewVersion = Version.Parse(previewVersionGroup.Value);
                    }
                }
                else if (previewVersionGroup.Success)
                {
                    Version previewVersion = Version.Parse(previewVersionGroup.Value);
                    if (previewVersion > runtimePreviewVersion)
                    {
                        runtimePreviewVersion = previewVersion;
                        runtimePath = sdkInfoMatch.Groups[pathGroup].Value;
                    }
                }
            }
        }

        if (runtimeVersion == null || string.IsNullOrEmpty(runtimePath))
        {
            throw new CbtException("Couldn't find any .Net 7 Sdk");
        }

        // --list-runtimes returns something like "C:\Program Files\dotnet\shared\Microsoft.NETCore.App"
        //  and I need to get "C:\Program Files\dotnet" dir from that
        DirectoryItem dotnetDir = new DirectoryItem(runtimePath).Parent.Parent;
        if (dotnetDir.Name != "dotnet")
        {
            throw new CbtException("Couldn't find dotnet directory");
        }

        return (dotnetDir, fullVersionString);
    }
}
