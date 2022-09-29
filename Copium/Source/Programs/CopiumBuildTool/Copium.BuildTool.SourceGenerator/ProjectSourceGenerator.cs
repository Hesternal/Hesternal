using System.Collections.Generic;
using System.Collections.Immutable;
using System.Linq;
using System.Threading;

using Microsoft.CodeAnalysis;

namespace Copium.BuildTool.SourceGenerator;


// https://www.thinktecture.com/net/roslyn-source-generators-introduction/


[Generator(LanguageNames.CSharp)]
internal sealed partial class ProjectSourceGenerator : IIncrementalGenerator
{
    private static int s_InitCount;
    private static int s_SelectCount;
    private static int s_EmitCount;


    private static Diagnostic _Diagnostic(string msg)
    {
        return Diagnostic.Create("ID_1", "CAT_1", msg, DiagnosticSeverity.Warning, DiagnosticSeverity.Warning, true, 1);
    }


    public void Initialize(IncrementalGeneratorInitializationContext context)
    {
        Interlocked.Increment(ref s_InitCount);

        var projectGenerators = context.CompilationProvider
            .SelectMany(_GetProjectGenerators)
            .Collect();

        context.RegisterSourceOutput(projectGenerators, _Execute);
    }


    private static IEnumerable<INamedTypeSymbol> _GetProjectGenerators(Compilation compilation, CancellationToken _)
    {
        if (compilation.AssemblyName == "Copium.BuildTool.Generators")
        {
            Interlocked.Increment(ref s_SelectCount);

            yield return compilation.GetTypeByMetadataName(ProjectGeneratorName.Vcxproj)!;
        }
    }

    private static void _Execute(SourceProductionContext context, ImmutableArray<INamedTypeSymbol> projectGenerators)
    {
        context.ReportDiagnostic(_Diagnostic($"_Execute | {projectGenerators.Length}"));
        if (projectGenerators.IsDefaultOrEmpty)
        {
            return;
        }

        SourceGenerationSpec spec = Parser.GetGenerationSpec(projectGenerators);
        Emitter.Emit(context, spec);
    }


    //private static bool _IsCompilerOptionsType(INamedTypeSymbol namedTypeSymbol, CancellationToken cancellationToken)
    //{
    //    return namedTypeSymbol.TypeKind == TypeKind.Class
    //        && namedTypeSymbol.DeclaredAccessibility is Accessibility.Public or Accessibility.Friend
    //        && namedTypeSymbol.GetAttributes().Any(static attributeData => attributeData.AttributeClass?.Name == k_AttributeClassName)
    //        ;//&& _IsPartial(namedTypeSymbol, cancellationToken);
    //}

//    private static IEnumerable<INamedTypeSymbol> _SelectAllAccessibleCopiumTypes(Compilation compilation, CancellationToken cancellationToken)
//    {
//        Interlocked.Increment(ref s_SelectCount);
//
//        return compilation.SourceModule.ReferencedAssemblySymbols
//            // Filter Copium assemblies
//            .Where(static assemblySymbol => assemblySymbol.Name.StartsWith(k_AssemblyNameStart))
//            // Get all namespaces
//            .SelectMany(static assemblySymbol => assemblySymbol.GlobalNamespace.GetNamespaceMembers())
//            // Filter Copium namespaces
//            .Where(static namespaceSymbol => namespaceSymbol.Name.StartsWith("Copium"))
//            .SelectMany(_SelectAllTypes)
//            // Filter CompilerOptions types
//            .Where(namedTypeSymbol => namedTypeSymbol is { TypeKind: TypeKind.Class, DeclaredAccessibility: Accessibility.Public or //Accessibility.Friend });
//    }

//    private static IEnumerable<INamedTypeSymbol> _SelectCompilerOptionsTypeSymbols(Compilation compilation, CancellationToken cancellationToken)
//    {
//        //Debugger.Launch();
//
//        //var t1 = compilation.GetTypesByMetadataName("Copium.BuildTool");
//        //var t2 = compilation.GetTypesByMetadataName("Copium.BuildTool.CompilerOptions");
//        //var t3 = compilation.GetTypesByMetadataName("Copium.BuildTool.CompilerOptionsAttribute");
//
//        return compilation.SourceModule.ReferencedAssemblySymbols
//            // Filter Copium assemblies
//            .Where(static assemblySymbol => assemblySymbol.Name.StartsWith(k_AssemblyNameStart))
//            // Get all namespaces
//            .SelectMany(static assemblySymbol => assemblySymbol.GlobalNamespace.GetNamespaceMembers())
//            // Filter Copium namespaces
//            .Where(static namespaceSymbol => namespaceSymbol.Name.StartsWith("Copium"))
//            .SelectMany(_SelectAllTypes)
//            // Filter CompilerOptions types
//            .Where(namedTypeSymbol => _IsCompilerOptionsType(namedTypeSymbol, cancellationToken));
//    }

//    private static bool _IsPartial(INamedTypeSymbol namedTypeSymbol, CancellationToken cancellationToken)
//    {
//        //Debugger.Launch();
//
//        if (namedTypeSymbol.DeclaringSyntaxReferences.Length > 1
//            || namedTypeSymbol.DeclaringSyntaxReferences.Any(s => ((ClassDeclarationSyntax)s.GetSyntax(cancellationToken)).IsPartial()))
//        {
//            return true;
//        }
//
//        return false;
//    }
//
//    // NOTE(v.matushkin): Without recursion?
//    private static IEnumerable<INamedTypeSymbol> _SelectAllTypes(INamespaceSymbol root)
//    {
//        foreach (INamespaceOrTypeSymbol namespaceOrTypeSymbol in root.GetMembers())
//        {
//            // NOTE(v.matushkin): There is no need for "else if" right? It's just one or the other?
//            if (namespaceOrTypeSymbol is INamedTypeSymbol typeSymbol)
//            {
//                yield return typeSymbol;
//            }
//            else if (namespaceOrTypeSymbol is INamespaceSymbol namespaceSymbol)
//            {
//                foreach (var nested in _SelectAllTypes(namespaceSymbol))
//                {
//                    yield return nested;
//                }
//            }
//        }
//    }
}
