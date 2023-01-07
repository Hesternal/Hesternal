using System.CommandLine;
using System.CommandLine.Parsing;
using System.IO;
using System.Linq;

namespace Hesternal.MathTool;


internal static class Program
{
    private static int Main(string[] args)
    {
        var privateDirOption = new Option<DirectoryInfo>("--private-dir", _ParseDirInfoOption, false, "Path to the private directory")
        {
            Arity = ArgumentArity.ExactlyOne,
            IsRequired = true,
        };
        var publicDirOption = new Option<DirectoryInfo>("--public-dir", _ParseDirInfoOption, false, "Path to the public directory")
        {
            Arity = ArgumentArity.ExactlyOne,
            IsRequired = true,
        };

        RootCommand rootCommand = new("HesternalMathTool")
        {
            privateDirOption, publicDirOption
        };
        rootCommand.SetHandler(_MathToolMain, privateDirOption, publicDirOption);

        return rootCommand.Invoke(args);
    }

    private static void _MathToolMain(DirectoryInfo privateDir, DirectoryInfo publicDir)
    {
        // MathTool.Generate(privateDir, publicDir);
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
}
