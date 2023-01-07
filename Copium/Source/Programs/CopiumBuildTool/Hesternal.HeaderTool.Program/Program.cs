using System;
using System.CommandLine;
using System.CommandLine.Parsing;
using System.IO;
using System.Linq;

using Copium.HeaderTool.Parser;

namespace Copium.HeaderTool.Program;


internal sealed class Program
{
    private static class OptionName
    {
        public const string DryRun                  = "--dry-run";
        public const string TargetDir               = "--target-dir";
        public const string ConfigDir               = "--config-dir";
        public const string TargetGeneratedDir      = "--target-generated-dir";
        public const string BaseGeneratedIncludeDir = "--base-include-dir";
    }

    private static class OptionDescription
    {
        public const string DryRun                  = "Parse headers, but do not generate anything";
        public const string TargetDir               = "Path to the target directory";
        public const string ConfigDir               = "Path to the config directory";
        public const string TargetGeneratedDir      = "Path to the generated target directory";
        public const string BaseGeneratedIncludeDir = "Base generated include directory";
    }

    private const string k_ToolDescription = "CopiumHeaderTool";


    private static int Main(string[] args)
    {
        Option<bool> dryRunOption = new(OptionName.DryRun, OptionDescription.DryRun)
        {
            Arity = ArgumentArity.ZeroOrOne,
            IsRequired = false,
        };

        Option<DirectoryInfo> targetDirOption = new(OptionName.TargetDir, _ParseDirInfoOption, false, OptionDescription.TargetDir)
        {
            Arity = ArgumentArity.ExactlyOne,
            IsRequired = true,
        };
        Option<DirectoryInfo> configDirOption = new(OptionName.ConfigDir, _ParseDirInfoOption, false, OptionDescription.ConfigDir)
        {
            Arity = ArgumentArity.ExactlyOne,
            IsRequired = true,
        };

        Option<DirectoryInfo?> targetGeneratedDirOption = new(OptionName.TargetGeneratedDir, OptionDescription.TargetGeneratedDir)
        {
            Arity = ArgumentArity.ExactlyOne,
            IsRequired = false,
        };
        Option<string?> baseIncludeDirOption = new(
            OptionName.BaseGeneratedIncludeDir, _ParseBaseIncludeDirOption, false, OptionDescription.BaseGeneratedIncludeDir)
        {
            Arity = ArgumentArity.ExactlyOne,
            IsRequired = false,
        };

        RootCommand rootCommand = new(k_ToolDescription)
        {
            dryRunOption, targetDirOption, configDirOption, targetGeneratedDirOption, baseIncludeDirOption
        };
        rootCommand.SetHandler(_HeaderToolMain, dryRunOption, targetDirOption, configDirOption, targetGeneratedDirOption, baseIncludeDirOption);
        rootCommand.AddValidator(commadResult =>
        {
            if (commadResult.GetValueForOption(dryRunOption) == false)
            {
                if (commadResult.GetValueForOption(targetGeneratedDirOption) == null)
                {
                    commadResult.ErrorMessage = OptionName.TargetDir + " should be specified when " + OptionName.DryRun + "=false";
                }
                else if (commadResult.GetValueForOption(baseIncludeDirOption) == null)
                {
                    commadResult.ErrorMessage = OptionName.BaseGeneratedIncludeDir + " should be specified when " + OptionName.DryRun + "=false";
                }
            }
        });

        return rootCommand.Invoke(args);
    }


    private static void _HeaderToolMain(bool dryRun, DirectoryInfo targetDir, DirectoryInfo configDir, DirectoryInfo? targetGeneratedDirm, string? baseIncludeDirOption)
    {
        HeaderToolOptions headerToolOptions;

        if (dryRun)
        {
            headerToolOptions = HeaderToolOptions.CreateDryRun(targetDir, configDir);
        }
        else
        {
            headerToolOptions = HeaderToolOptions.Create(targetDir, configDir, targetGeneratedDirm, baseIncludeDirOption);
        }

        try
        {
            HeaderTool headerTool = new(headerToolOptions);
            headerTool.Generate(_GetHeaderTaskItems(targetDir));
        }
        catch (ParserException e)
        {
            Console.WriteLine($"[{e.FilePath}({e.Line}, {e.Column})] {e.Message}");
            throw;
        }
    }

    private static HeaderToolTaskItem[] _GetHeaderTaskItems(DirectoryInfo targetDir)
    {
        return targetDir.EnumerateFiles("*.ixx", SearchOption.AllDirectories)
            .Select(headerFile => new HeaderToolTaskItem(headerFile))
            .ToArray();
    }


    private static DirectoryInfo _ParseDirInfoOption(ArgumentResult dirInfoArgumentResult)
    {
        string dirPath = dirInfoArgumentResult.Tokens.Single().Value;
        DirectoryInfo dirInfo = new(dirPath);

        if (dirInfo.Exists == false)
        {
            dirInfoArgumentResult.ErrorMessage = $"Directory doesn't exist --{dirInfoArgumentResult.Argument.Name}={dirPath}";
        }

        return dirInfo;
    }

    private static string _ParseBaseIncludeDirOption(ArgumentResult baseIncludeDirArgumentResult)
    {
        string baseIncludeDir = baseIncludeDirArgumentResult.Tokens.Single().Value;

        if (baseIncludeDir == string.Empty)
        {
            baseIncludeDirArgumentResult.ErrorMessage = "Base include dir was empty";
        }
        else if (baseIncludeDir.All(char.IsLetter) == false)
        {
            baseIncludeDirArgumentResult.ErrorMessage = "Base include dir should contain only letters";
        }

        return baseIncludeDir;
    }
}
