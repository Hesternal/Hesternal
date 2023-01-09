using Microsoft.CodeAnalysis;

namespace Hesternal.BuildTool.SourceGenerator;


public static class INamedTypeSymbolExtensions
{
    public static string ToFullyQualifiedName(this INamedTypeSymbol namedTypeSymbol)
    {
        return namedTypeSymbol.ToDisplayString(SymbolDisplayFormat.FullyQualifiedFormat);
    }
}
