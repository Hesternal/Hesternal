using Microsoft.CodeAnalysis;

namespace Hesternal.BuildTool.SourceGenerator;


internal static class HesternalDiagnosticDescriptors
{
    public static readonly DiagnosticDescriptor CompilerOptionsMustBePartial
        = new("COP001", "CompilerOptions class must be partial", "The '{0}' must be partial",
            "HesternalAnalyzer", DiagnosticSeverity.Error, true);
}
