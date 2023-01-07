using System;
using System.IO;
using System.Reflection;
using System.Text;

using Hesternal.BuildTool.External;
using Hesternal.Core.Net7;

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.Emit;
using Microsoft.CodeAnalysis.Text;

namespace Hesternal.BuildTool.Build;


internal sealed class CSharpCompiler
{
    private static readonly Lazy<MetadataReference[]> s_MetadataReferences = new(_CreateMetadataReferences, false);

    private readonly CSharpParseOptions m_parseOptions;
    private readonly CSharpCompilationOptions m_compilationOptions;


    public CSharpCompiler()
    {
        m_parseOptions = new(LanguageVersion.Preview, DocumentationMode.None, SourceCodeKind.Regular, null);

        m_compilationOptions = new(
            OutputKind.DynamicallyLinkedLibrary,
            false,
            optimizationLevel: OptimizationLevel.Release,
            warningLevel: 7
        );
    }


    public Assembly Compile(string assemblyName, FileItem[] sourceFiles)
    {
        SyntaxTree[] syntaxTrees = new SyntaxTree[sourceFiles.Length];

        for (int i = 0; i < sourceFiles.Length; ++i)
        {
            FileItem sourceFile = sourceFiles[i];
            SourceText sourceText;
            using (FileStream sourceReader = sourceFile.OpenRead())
            {
                sourceText = SourceText.From(sourceReader);
            }
            syntaxTrees[i] = CSharpSyntaxTree.ParseText(sourceText, m_parseOptions, sourceFile.FullPath);
        }

        CSharpCompilation csharpCompilation = CSharpCompilation.Create(assemblyName, syntaxTrees, s_MetadataReferences.Value, m_compilationOptions);

        using (MemoryStream dllStream = new())
        {
            EmitResult emitResult = csharpCompilation.Emit(dllStream, null);

            if (emitResult.Success == false)
            {
                StringBuilder errorString = new("Failed compilation for ");
                errorString.Append(assemblyName);

                foreach (Diagnostic diagnostic in emitResult.Diagnostics)
                {
                    if (diagnostic.Severity == DiagnosticSeverity.Error)
                    {
                        errorString.AppendLine(diagnostic.ToString());
                    }
                }

                throw new CbtException(errorString.ToString());
            }

            byte[] assemblyBytes = dllStream.GetBuffer();

            //FileItem dllFile = HesternalGlobal.Build.BinDotnetDir.MakeSubFileItem(assemblyName + ".dll");
            //dllFile.WriteAllBytes(assemblyBytes);

            return Assembly.Load(assemblyBytes);
        }
    }


    private static MetadataReference[] _CreateMetadataReferences()
    {
        string dotnetRefDirPath = DotnetHelper.Instance.RefAssembliesDir.FullPath;
        return new MetadataReference[]
        {
            MetadataReference.CreateFromFile(Path.Join(dotnetRefDirPath, "System.Collections.dll")),
            MetadataReference.CreateFromFile(Path.Join(dotnetRefDirPath, "System.Linq.dll")),
            MetadataReference.CreateFromFile(Path.Join(dotnetRefDirPath, "System.Runtime.dll")),
            MetadataReference.CreateFromFile(typeof(DirectoryItem).Assembly.Location), // Hesternal.Core.Net7
            MetadataReference.CreateFromFile(typeof(Project).Assembly.Location), // Hesternal.BuildTool
        };
    }
}
