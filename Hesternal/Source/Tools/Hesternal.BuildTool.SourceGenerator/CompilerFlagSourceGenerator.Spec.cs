using System.Collections.Generic;

namespace Hesternal.BuildTool.SourceGenerator;


internal sealed partial class CompilerFlagSourceGenerator
{
    private sealed class EnumFlagOptionSpec
    {
        public string Name { get; init; }
        public string? Flag { get; init; }
        public string VcxprojName { get; init; }
    }


    private sealed class EnumFlagSpec
    {
        public string EnumName { get; init; }
        public string EnumFullyQualifiedName { get; init; }
        public readonly List<EnumFlagOptionSpec> Members = new();
    }
}
