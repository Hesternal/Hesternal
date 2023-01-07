using Microsoft.CodeAnalysis;

namespace Copium.BuildTool.SourceGenerator;


internal static class CopiumDiagnosticDescriptors
{
    public static readonly DiagnosticDescriptor CompilerOptionsMustBePartial
        = new("COP001", "CompilerOptions class must be partial", "The '{0}' must be partial",
            "CopiumAnalyzer", DiagnosticSeverity.Error, true);
}
