using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;

using Copium.BuildTool.BuildRules;
using Copium.BuildTool.Core;
using Copium.BuildTool.External;

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.Emit;
using Microsoft.CodeAnalysis.Text;
using Microsoft.Extensions.Logging;

// https://stackoverflow.com/questions/6258160/unloading-the-assembly-loaded-with-assembly-loadfrom

namespace Copium.BuildTool.Build;


internal sealed class BuildRulesAssembly
{
    private readonly Assembly m_assembly;
    private string AssemblyName => m_assembly.GetName().Name!;

    private readonly Type                     m_targetRulesType;
    private readonly Dictionary<string, Type> m_moduleRulesTypes;


    // public BuildRulesAssembly(string buildRulesAssemblyPath)
    // {
    //     m_assembly = Assembly.LoadFrom(buildRulesAssemblyPath);
    //     (m_targetRulesType, m_moduleRulesTypes) = _GetBuildRulesTypes();
    // }

    public BuildRulesAssembly(Assembly buildRulesAssembly)
    {
        m_assembly = buildRulesAssembly;
        (m_targetRulesType, m_moduleRulesTypes) = _GetBuildRulesTypes();
    }


    public TargetBuildRules CreateTargetBuildRules(string targetName, TargetBuildInfo targetBuildInfo)
    {
        if (m_targetRulesType.Name != targetName)
        {
            throw new CbtException($"Assembly '{AssemblyName}.dll', expected TargetBuildRules class with a name: '{targetName}', got: '{m_targetRulesType.Name}'");
        }

        ConstructorInfo constructor = m_targetRulesType.GetConstructor(new[] { typeof(TargetBuildInfo) })!;

        TargetBuildRules targetBuildRules;

        try
        {
            targetBuildRules = (TargetBuildRules)constructor.Invoke(new[] { targetBuildInfo });
        }
        catch (Exception)
        {
            throw new CbtException($"Assembly '{AssemblyName}.dll', type: '{m_targetRulesType.Name}', unable to instantiate an instance of a class");
        }

        return targetBuildRules;
    }

    public ModuleBuildRules CreateModuleBuildRules(string moduleName, TargetBuildRules targetBuildRules)
    {
        Type? moduleBuildRulesType;

        if (m_moduleRulesTypes.TryGetValue(moduleName, out moduleBuildRulesType) == false)
        {
            throw new CbtException($"Assembly '{AssemblyName}.dll', couldn't find ModuleBuildRules for module: {moduleName}");
        }

        ConstructorInfo constructor = moduleBuildRulesType.GetConstructor(new[] { typeof(TargetBuildRules) })!;

        try
        {
            return (ModuleBuildRules)constructor.Invoke(new[] { targetBuildRules });
        }
        catch (Exception)
        {
            throw new CbtException($"Assembly '{AssemblyName}.dll', type: '{m_targetRulesType.Name}', unable to instantiate an instance of aclass");
        }
    }


    private (Type, Dictionary<string, Type>) _GetBuildRulesTypes()
    {
        Type? targetRulesType = null;
        List<Type> moduleRulesTypes = new();

        Type targetBuildRulesBaseType = typeof(TargetBuildRules);
        Type moduleBuildRulesBaseType = typeof(ModuleBuildRules);
        Type[] moduleBuildRulesConstructorTypes = new[] { typeof(TargetBuildRules) };

        foreach (Type type in m_assembly.ExportedTypes)
        {
            if (type.IsClass == false)
            {
                Log.Logger.LogWarning("Assembly '{AssemblyName}.dll', type: '{TypeName}' is not a class type", AssemblyName, type.Name);
                continue;
            }

            Type? baseType = type.BaseType;

            if (type.BaseType == null)
            {
                Log.Logger.LogWarning("Assembly '{AssemblyName}.dll', type: '{TypeName}' inherits nothing", AssemblyName, type.Name);
                continue;
            }

            if (baseType == targetBuildRulesBaseType)
            {
                if (type.GetConstructor(new Type[] { typeof(TargetBuildInfo) }) == null)
                {
                    throw new CbtException(
                        $"Assembly '{AssemblyName}.dll', type: '{type.Name}', couldn't find a constructor which takes an argument of type 'TargetBuildInfo'");
                }

                targetRulesType = type;
            }
            else if (baseType == moduleBuildRulesBaseType)
            {
                if (type.GetConstructor(moduleBuildRulesConstructorTypes) == null)
                {
                    throw new CbtException(
                        $"Assembly '{AssemblyName}.dll', type: '{type.Name}', couldn't find a constructor which takes an argument of type 'TargetBuildRules'");
                }

                moduleRulesTypes.Add(type);
            }
            else
            {
                Log.Logger.LogWarning(
                    "Assembly '{AssemblyName}.dll', type: '{TypeName}' inherits neither 'TargetBuildRules' nor 'ModuleBuildRules'",
                    AssemblyName, type.Name);
            }
        }

        if (targetRulesType == null)
        {
            throw new CbtException($"Assembly '{AssemblyName}.dll', couldn't find a Type that inherits 'TargetBuildRules'");
        }
        if (moduleRulesTypes.Count == 0)
        {
            throw new CbtException($"Assembly '{AssemblyName}.dll', couldn't find a Type that inherits 'ModuleBuildRules'");
        }

        return (targetRulesType, moduleRulesTypes.ToDictionary(type => type.Name, type => type));
    }


    public static BuildRulesAssembly Compile(RulesTarget rulesTarget)
    {
        Log.Logger.LogInformation("Compiling {rulesTargetName} assembly", rulesTarget.TargetName);

        CSharpParseOptions csharpParseOptions = new(LanguageVersion.Preview, DocumentationMode.None, SourceCodeKind.Regular, null);
        SyntaxTree[] syntaxTrees = new SyntaxTree[rulesTarget.SourceFiles.Length];

        for (int i = 0; i < rulesTarget.SourceFiles.Length; ++i)
        {
            FileItem sourceFile = rulesTarget.SourceFiles[i];
            SourceText sourceText;
            using (FileStream sourceReader = sourceFile.OpenRead())
            {
                sourceText = SourceText.From(sourceReader);
            }
            syntaxTrees[i] = CSharpSyntaxTree.ParseText(sourceText, csharpParseOptions, sourceFile.FullPath);
        }

        CSharpCompilationOptions csharpCompilationOptions = new(
            OutputKind.DynamicallyLinkedLibrary,
            false,
            optimizationLevel: OptimizationLevel.Release,
            warningLevel: 7
        );

        string dotnetRefDirPath = DotnetHelper.Instance.RefAssembliesDir.FullPath;
        MetadataReference[] metadataReferences = new[]
        {
            MetadataReference.CreateFromFile(Path.Join(dotnetRefDirPath, "System.Collections.dll")),
            MetadataReference.CreateFromFile(Path.Join(dotnetRefDirPath, "System.Runtime.dll")),
            MetadataReference.CreateFromFile(typeof(TargetBuildRules).Assembly.Location),
        };

        CSharpCompilation csharpCompilation = CSharpCompilation.Create(rulesTarget.TargetName, syntaxTrees, metadataReferences, csharpCompilationOptions);

        using (MemoryStream dllStream = new())
        {
            EmitResult emitResult = csharpCompilation.Emit(dllStream, null);

            if (emitResult.Success == false)
            {
                StringBuilder errorString = new("Failed compilation for ");
                errorString.Append(rulesTarget.TargetName);

                foreach (Diagnostic diagnostic in emitResult.Diagnostics)
                {
                    if (diagnostic.Severity == DiagnosticSeverity.Error)
                    {
                        errorString.AppendLine(diagnostic.ToString());
                    }
                }

                throw new CbtException(errorString.ToString());
            }

            FileItem dllFile = CopiumGlobal.Build.BinDotnetDir.MakeSubFileItem(rulesTarget.TargetName + ".dll");
            dllFile.WriteAllBytes(dllStream.GetBuffer());

            return new BuildRulesAssembly(Assembly.Load(dllStream.GetBuffer()));
        }
    }
}
