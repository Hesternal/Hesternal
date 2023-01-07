using System.CommandLine;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

using Hesternal.BuildTool.CommandExecutor;
using Hesternal.BuildTool.External;
using Hesternal.CSharp.Net7;

// using Microsoft.Build.Locator;
using Microsoft.Extensions.Logging;

namespace Hesternal.BuildTool;


internal static class Program
{
    private static int Main(string[] args)
    {
        // MSBuildLocator.RegisterDefaults();

        var projectDirOption = new Option<DirectoryItem>("--project-dir", _ParseDirItemOption, false, "Path to the project directory")
        {
            Arity = ArgumentArity.ExactlyOne,
            IsRequired = true,
        };

        Command generateCommand = new("generate", "Generate project files")
        {
            projectDirOption
        };
        generateCommand.SetHandler((projectDir) =>
            {
                _HesternalBuildToolMain(_GenerateCommandHandler(projectDir));
            },
            projectDirOption);

        // var chtConfigFileOption = new Option<FileInfo>("--chtConfig", _ParseFileInfoOption, false, "Path to the HesternalHeaderTool config path")
        // {
        //     Arity = ArgumentArity.ExactlyOne,
        //     IsRequired = true,
        // };

        RootCommand rootCommand = new("HesternalBuildTool");
        rootCommand.AddCommand(generateCommand);

        return rootCommand.Invoke(args);
    }


    private static ICommandExecutor _GenerateCommandHandler(DirectoryItem projectDir)
    {
        return new GenerateCommandExecutor(projectDir);
    }


    private static void _HesternalBuildToolMain(ICommandExecutor commandExecutor)
    {
        Log.Init(LogLevel.Trace);
        TaskPool.Init();

        try
        {
            // Conan.ValidateInstallation();
            // _ = DotnetHelper.Instance;

            Task dotnetTask = TaskPool.Factory.StartNew(() => DotnetHelper.Instance);
            Conan.ValidateInstallation();
            dotnetTask.Wait();

            commandExecutor.Execute();
        }
        //catch (HbtException e)
        //{
        //    Log.Logger.LogCritical(e.Message);
        //}
        //catch (AggregateException e)
        //{
        //    e.Handle(innerException =>
        //    {
        //        if (innerException is HbtException)
        //        {
        //            Log.Logger.LogCritical(innerException.Message);
        //            return true;
        //        }
        //        return false;
        //    });
        //}
        finally
        {
            Log.Shutdown();
        }
    }


    private static DirectoryItem _ParseDirItemOption(System.CommandLine.Parsing.ArgumentResult dirInfoArgumentResult)
    {
        string dirPath = dirInfoArgumentResult.Tokens.Single().Value;
        DirectoryInfo dirInfo = new(dirPath);
    
        if (dirInfo.Exists == false)
        {
            dirInfoArgumentResult.ErrorMessage = $"Directory doesn't exist --{dirInfoArgumentResult.Argument.Name}={dirPath}";
        }

        return new DirectoryItem(dirInfo);
    }

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
