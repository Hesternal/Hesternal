//using System.Collections.Immutable;
//
//using Microsoft.CodeAnalysis;
//using Microsoft.CodeAnalysis.CSharp.Syntax;
//using Microsoft.CodeAnalysis.Diagnostics;
//
//namespace Copium.BuildTool.SourceGenerator;
//
//
//[DiagnosticAnalyzer(LanguageNames.CSharp)]
//public sealed class CopiumDiagnosticAnalyzer : DiagnosticAnalyzer
//{
//    public override ImmutableArray<DiagnosticDescriptor> SupportedDiagnostics => ImmutableArray.Create(CopiumDiagnosticDescriptors.CompilerOptionsMustBePartial);
//
//
//    public override void Initialize(AnalysisContext context)
//    {
//        context.ConfigureGeneratedCodeAnalysis(GeneratedCodeAnalysisFlags.None);
//        context.EnableConcurrentExecution();
//
//        context.RegisterSymbolAction(_AnalyzeNamedType, SymbolKind.NamedType);
//    }
//
//
//    private static void _AnalyzeNamedType(SymbolAnalysisContext context)
//    {
//        INamedTypeSymbol namedTypeSymbol = (INamedTypeSymbol)context.Symbol;
//
//        if (VsCppProjectSourceGenerator.IsCompilerOptions(namedTypeSymbol) == false)
//        {
//            return;
//        }
//
//        foreach (SyntaxReference syntaxReference in namedTypeSymbol.DeclaringSyntaxReferences)
//        {
//            if (syntaxReference.GetSyntax() is not ClassDeclarationSyntax classDeclarationSyntax
//                || classDeclarationSyntax.IsPartial())
//            {
//                continue;
//            }
//
//            context.ReportDiagnostic(Diagnostic.Create(CopiumDiagnosticDescriptors.CompilerOptionsMustBePartial,
//                classDeclarationSyntax.Identifier.GetLocation(),
//                namedTypeSymbol.Name));
//        }
//    }
//}
