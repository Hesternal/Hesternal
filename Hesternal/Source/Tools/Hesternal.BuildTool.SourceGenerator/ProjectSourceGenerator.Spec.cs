using System.Collections.Generic;

using Microsoft.CodeAnalysis;

namespace Hesternal.BuildTool.SourceGenerator;


internal sealed partial class ProjectSourceGenerator
{
    private sealed class ProjectGeneratorSpec
    {
        public readonly List<INamedTypeSymbol> BuildSystems = new();
        public readonly List<INamedTypeSymbol> Compilers = new();
        public readonly List<INamedTypeSymbol> Linkers = new();
    }


    private sealed class SourceGenerationSpec
    {
        public ProjectGeneratorSpec Vcxproj { get; init; }
    }
}
