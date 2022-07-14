using System;
using System.CommandLine;
using System.Threading.Tasks;

using Copium.BuildTool.CommandExecutor;
using Copium.BuildTool.Core;
using Copium.BuildTool.External;

// using Microsoft.Build.Locator;
using Microsoft.Extensions.Logging;

namespace Copium.BuildTool;


internal static class Program
{
    private static int Main(string[] args)
    {
        // MSBuildLocator.RegisterDefaults();

        Command generateCommand = new("generate", "Generate project files");
        generateCommand.SetHandler(() => _CopiumBuildToolMain(_GenerateCommandHandler()));

        // var projectsDirOption = new Option<DirectoryInfo>("--projectsDir", _ParseDirInfoOption, false, "Path to the projects directory")
        // {
        //     Arity = ArgumentArity.ExactlyOne,
        //     IsRequired = true,
        // };
        // var chtConfigFileOption = new Option<FileInfo>("--chtConfig", _ParseFileInfoOption, false, "Path to the CopiumHeaderTool config path")
        // {
        //     Arity = ArgumentArity.ExactlyOne,
        //     IsRequired = true,
        // };

        RootCommand rootCommand = new("CopiumBuildTool");
        rootCommand.AddCommand(generateCommand);

        return rootCommand.Invoke(args);
    }


    private static ICommandExecutor _GenerateCommandHandler()
    {
        return new GenerateCommandExecutor();
    }


    private static void _CopiumBuildToolMain(ICommandExecutor commandExecutor)
    {
        Log.Init(LogLevel.Trace);
        TaskPool.Init();

        try
        {
            // NOTE(v.matushkin): Trigger constructors at the start
            _ = CopiumGlobal.Build;

            // Conan.ValidateInstallation();
            // _ = DotnetHelper.Instance;

            Task dotnetTask = TaskPool.Factory.StartNew(() => DotnetHelper.Instance);
            Conan.ValidateInstallation();
            dotnetTask.Wait();

            commandExecutor.Execute();
        }
        catch (CbtException e)
        {
            Log.Logger.LogCritical(e.Message);
        }
        catch (AggregateException e)
        {
            e.Handle(innerException =>
            {
                if (innerException is CbtException)
                {
                    Log.Logger.LogCritical(innerException.Message);
                    return true;
                }
                return false;
            });
        }
        finally
        {
            Log.Shutdown();
        }
    }


    // private static DirectoryInfo _ParseDirInfoOption(System.CommandLine.Parsing.ArgumentResult dirInfoArgumentResult)
    // {
    //     string dirPath = dirInfoArgumentResult.Tokens.Single().Value;
    //     DirectoryInfo dirInfo = new(dirPath);
    // 
    //     if (dirInfo.Exists == false)
    //     {
    //         dirInfoArgumentResult.ErrorMessage = $"Directory doesn't exist --{dirInfoArgumentResult.Argument.Name}={dirPath}";
    //     }
    // 
    //     return dirInfo;
    // }

    // private static FileInfo _ParseFileInfoOption(System.CommandLine.Parsing.ArgumentResult fileInfoArgumentResult)
    // {
    //     string filePath = fileInfoArgumentResult.Tokens.Single().Value;
    //     FileInfo fileInfo = new(filePath);
    // 
    //     if (fileInfo.Exists == false)
    //     {
    //         fileInfoArgumentResult.ErrorMessage = $"File doesn't exist --{fileInfoArgumentResult.Argument.Name}={filePath}";
    //     }
    // 
    //     return fileInfo;
    // }
}
