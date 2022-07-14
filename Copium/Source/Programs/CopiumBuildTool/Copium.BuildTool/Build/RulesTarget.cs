using System.Collections.Generic;

using Copium.BuildTool.BuildRules;
using Copium.BuildTool.Core;

namespace Copium.BuildTool.Build;


internal sealed class RulesTarget : Target
{
    private readonly string m_targetName;

    /// <summary>
    /// TARGET_NAME.Target.cs file and MODULE_NAME.Module.cs files
    /// </summary>
    public readonly FileItem[] SourceFiles;


    public override string TargetName => m_targetName;


    public RulesTarget(DirectoryItem targetDir, FileItem targetRulesFile)
        : base(targetDir)
    {
        m_targetName = targetDir.Name + "Rules";
        SourceFiles = _FindModuleRulesFiles(targetRulesFile);
    }


    private FileItem[] _FindModuleRulesFiles(FileItem targetRulesFile)
    {
        List<FileItem> sourceFiles = new() { targetRulesFile };

        foreach (DirectoryItem moduleDir in m_targetDir.EnumerateDirectories())
        {
            FileItem moduleRulesItem = moduleDir.MakeSubFileItem(moduleDir.Name + ModuleBuildRules.FileExtension);
            if (moduleRulesItem.Exists == false)
            {
                throw new CbtException($"Couldn't find '{moduleRulesItem.FileName}' file for '{moduleDir.Name}' module");
            }

            sourceFiles.Add(moduleRulesItem);
        }

        return sourceFiles.ToArray();
    }
}
