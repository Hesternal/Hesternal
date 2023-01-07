using System;
using System.Collections.Generic;
using System.Diagnostics;

using Hesternal.Core.Net7;

using Microsoft.Extensions.Logging;

namespace Hesternal.BuildTool.External;


internal sealed class Conan
{
    private static class ConanStartInfo
    {
        private const string k_FileName = "conan";
        private const string k_OptionVersion = "--version";

        public static ProcessStartInfo Version()
        {
            ProcessStartInfo conanStartIfno = new(k_FileName, k_OptionVersion);
            conanStartIfno.UseShellExecute = false;
            conanStartIfno.RedirectStandardOutput = true;
            conanStartIfno.RedirectStandardError = true;
            conanStartIfno.CreateNoWindow = true;

            return conanStartIfno;
        }

        // public static ProcessStartInfo Install()
        // {
        //     ProcessStartInfo conanStartIfno = new(k_FileName);
        //     conanStartIfno.UseShellExecute = false;
        //     conanStartIfno.RedirectStandardOutput = true;
        //     conanStartIfno.CreateNoWindow = false;
        //     conanStartIfno.WorkingDirectory = HesternalGlobal.RootDir.FullPath;
        // 
        //     return conanStartIfno;
        // }
    }


    private static readonly Version s_ConanRequiredVersion = new(1, 55, 0);


    public static void ValidateInstallation()
    {
        string? conanVersionOutput = null;
        List<string>? errorOutput = null;

        using (Process conanProcess = new())
        {
            conanProcess.StartInfo = ConanStartInfo.Version();

            conanProcess.OutputDataReceived += (object sender, DataReceivedEventArgs e) =>
            {
                if (e.Data != null)
                {
                    if (conanVersionOutput == null)
                    {
                        conanVersionOutput = e.Data;
                    }
                    else
                    {
                        throw new HbtException("Didn't expect to get more than one line of output for \"conan --version\" command");
                    }
                }
            };
            conanProcess.ErrorDataReceived += (object sender, DataReceivedEventArgs e) =>
            {
                if (e.Data != null)
                {
                    (errorOutput ??= new()).Add(e.Data);
                }
            };

            conanProcess.Start();
            conanProcess.BeginOutputReadLine();
            conanProcess.BeginErrorReadLine();
            conanProcess.WaitForExit();
        }

        if (conanVersionOutput == null)
        {
            throw new HbtException("\"conan --version\" output was empty");
        }

        string[] outputParts = conanVersionOutput.Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
        Version? conanVersion;

        if (outputParts.Length != 3 || outputParts[0] != "Conan" || outputParts[1] != "version"
            || Version.TryParse(outputParts[2], out conanVersion) == false)
        {
            throw new HbtException("Couldn't parse \"conan --version\" output");
        }

        if (s_ConanRequiredVersion != conanVersion)
        {
            throw new HbtException($"Required conan version is {s_ConanRequiredVersion}, found {conanVersion} instead");
        }

        Log.Logger.LogInformation("Found conan {version}", s_ConanRequiredVersion.ToString());
    }


    // public static void Install(DirectoryItem conanDir, BuildMatrix buildMatrix)
    // {
    //     // conan install . -if build\conan\vs -s build_type=Debug -o development=False
    // 
    //     List<string>? errorOutput = null;
    // 
    //     ProcessStartInfo processStartInfo = new();
    //     processStartInfo.UseShellExecute = true;
    //     processStartInfo.RedirectStandardError = true;
    //     processStartInfo.WorkingDirectory = HesternalGlobal.RootDir.FullPath;
    // 
    //     using (Process conanProcess = new())
    //     {
    //         conanProcess.StartInfo = processStartInfo;
    // 
    //         conanProcess.ErrorDataReceived += (object sender, DataReceivedEventArgs e) =>
    //         {
    //             if (e.Data != null)
    //             {
    //                 (errorOutput ??= new()).Add(e.Data);
    //             }
    //         };
    // 
    //         conanProcess.StandardOutput.ReadToEnd()
    // 
    //         conanProcess.Start();
    //         conanProcess.BeginErrorReadLine();
    //         conanProcess.WaitForExit();
    //     }
    // }
}
