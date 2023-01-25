/*
using System.Threading;

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp.Syntax;

namespace Hesternal.BuildTool.SourceGenerator;


[Generator(LanguageNames.CSharp)]
internal sealed partial class CompilerFlagSourceGenerator : IIncrementalGenerator
{
    // NOTE(v.matushkin): This counters doesn't work for this generator like for ProjectSourceGenerator
    //   because it runs for every enum individually
    private static int s_InitCount;
    private static int s_EmitCount;


    public void Initialize(IncrementalGeneratorInitializationContext context)
    {
        Interlocked.Increment(ref s_InitCount);

        var compilerFlagEnums = context.SyntaxProvider
            .ForAttributeWithMetadataName(
                HesternalInfo.Attribute.CompilerFlag.Enum.FullName,
                predicate: static (node, _) => node is EnumDeclarationSyntax,
                transform: static (context, _) => context.TargetSymbol as INamedTypeSymbol)
            .Where(static typeSymbol => typeSymbol is not null);

        context.RegisterSourceOutput(compilerFlagEnums, _Execute);
    }

    private static void _Execute(SourceProductionContext context, INamedTypeSymbol? compilerFlagEnum)
    {
        context.ReportDiagnostic(_Diagnostic($"CompilerFlagSourceGenerator::_Execute | {compilerFlagEnum is not null}"));
        if (compilerFlagEnum is not null)
        {
            EnumFlagSpec spec = Parser.GetGenerationSpec(compilerFlagEnum);
            Emitter.Emit(context, spec);
        }
    }


    private static Diagnostic _Diagnostic(string msg)
    {
        return Diagnostic.Create("ID_1", "CAT_1", msg, DiagnosticSeverity.Warning, DiagnosticSeverity.Warning, true, 1);
    }
}
*/
