using System;
using System.Diagnostics;
using System.Reflection;

using Copium.BuildTool.Build;
using Copium.BuildTool.Generators.VisualStudio;
using Copium.Core.Net7;

using Microsoft.Extensions.Logging;

namespace Copium.BuildTool.CommandExecutor;


internal sealed class GenerateCommandExecutor : ICommandExecutor
{
    private readonly DirectoryItem m_projectDir;


    public GenerateCommandExecutor(DirectoryItem projectDir)
    {
        m_projectDir = projectDir;
    }


    public void Execute()
    {
        Stopwatch executionTimer = Stopwatch.StartNew();

        Assembly solutionAssembly = _CompileSolutionAssembly(_FindProjectFiles(m_projectDir));
        Solution solution = _GetSolution(solutionAssembly);
        solution.ResolveDependencies();

        foreach (CppProject cppProject in solution.CppProjects)
        {
            cppProject.ConfigureProject();
        }

        Log.Logger.LogInformation("Generating solution ...");

        VsSolutionGenerator.Generate(solution);

        executionTimer.Stop();
        Log.Logger.LogInformation("Done.");
        Log.Logger.LogInformation("Generate command execution time: {time}ms", executionTimer.ElapsedMilliseconds);
    }


    private static FileItem[] _FindProjectFiles(DirectoryItem dir)
    {
        const string searchPattern = "*.hbt.cs";

        return dir.GetFiles(searchPattern);
    }

    private static Assembly _CompileSolutionAssembly(FileItem[] projectFiles)
    {
        // TODO(v.matushkin): Name hardcoded
        return new CSharpCompiler().Compile("CopiumProjects", projectFiles);
    }

    private static Solution _GetSolution(Assembly solutionAssembly)
    {
        Type mainType = solutionAssembly.GetType("Main")!;
        MethodInfo entryPointMethod = mainType.GetMethod("EntryPoint", BindingFlags.Static | BindingFlags.NonPublic, Type.EmptyTypes)!;

        return (entryPointMethod.Invoke(null, null)! as Solution)!;
    }
}
