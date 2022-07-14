using System.Collections.Generic;
using System.Diagnostics;

using Copium.BuildTool.Build;
using Copium.BuildTool.BuildRules;
using Copium.BuildTool.Core;
using Copium.BuildTool.Project;
using Copium.BuildTool.Project.VisualStudio;

using Microsoft.Extensions.Logging;

namespace Copium.BuildTool.CommandExecutor;


internal sealed class GenerateCommandExecutor : ICommandExecutor
{
    public void Execute()
    {
        Stopwatch executionTimer = Stopwatch.StartNew();

        CopiumGlobal.CreateBuildDirs();

        TargetBuildInfo targetBuidlInfo = new(BuildOS.Windows, BuildCompiler.Msvc);

        TargetCollection targetCollection = _FindTargets(targetBuidlInfo);

        Log.Logger.LogInformation("Generating solution ...");

        VsSolutionGenerator.Generate(new BuildMatrix(), targetCollection);

        executionTimer.Stop();

        Log.Logger.LogInformation("Done.");
        Log.Logger.LogInformation("Generate command execution time: {time}ms", executionTimer.ElapsedMilliseconds);
    }


    private static TargetCollection _FindTargets(TargetBuildInfo targetBuidlInfo)
    {
        TargetCollection targetCollection = new();

        foreach (DirectoryItem dir in CopiumGlobal.Copium.SourceDir.EnumerateDirectories())
        {
            if (dir.Name == "Programs")
            {
                targetCollection.ProgramSolutionFiles = _GetProgramSolutionFiles(dir);
            }
            else
            {
                FileItem targetFile = dir.MakeSubFileItem(dir.Name + TargetBuildRules.FileExtension);

                if (targetFile.Exists)
                {
                    RulesTarget rulesTarget = new(dir, targetFile);
                    BuildRulesAssembly rulesAssembly = BuildRulesAssembly.Compile(rulesTarget);
                    CppTarget target = new(targetBuidlInfo, dir, rulesAssembly);

                    targetCollection.RulesTargets.Add(rulesTarget);
                    targetCollection.CppTargets.Add(target);
                }
                else
                {
                    Log.Logger.LogWarning("Skipping source directory without Target.cs file [{programDir}]", dir.FullPath);
                }
            }
        }

        return targetCollection;
    }

    private static FileItem[] _GetProgramSolutionFiles(DirectoryItem programsDir)
    {
        const string solutionSearchPattern = "*" + VsFileExtension.Solution;

        List<FileItem> programSolutions = new();

        foreach (DirectoryItem programDir in programsDir.EnumerateDirectories())
        {
            FileItem[] programSolutionFiles = programDir.GetFiles(solutionSearchPattern, System.IO.SearchOption.TopDirectoryOnly);

            if (programSolutionFiles.Length == 0)
            {
                Log.Logger.LogWarning("Skipping program directory without .sln file [{programDir}]", programDir.FullPath);
                continue;
            }
            if (programSolutionFiles.Length > 1)
            {
                Log.Logger.LogWarning("Skipping program directory with {filesCount} .sln files [{programDir}]", programSolutionFiles.Length, programDir.FullPath);
                continue;
            }

            programSolutions.Add(programSolutionFiles[0]);
        }

        return programSolutions.ToArray();
    }
}
