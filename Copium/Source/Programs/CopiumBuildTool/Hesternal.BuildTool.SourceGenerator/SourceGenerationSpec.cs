using System.Collections.Generic;
using System.ComponentModel;

using Microsoft.CodeAnalysis;


namespace System.Runtime.CompilerServices
{
    [EditorBrowsable(EditorBrowsableState.Never)]
    internal class IsExternalInit { }
}


namespace Copium.BuildTool.SourceGenerator
{
    internal sealed class ProjectGeneratorSpec
    {
        public readonly List<INamedTypeSymbol> BuildSystems = new();
        public readonly List<INamedTypeSymbol> Compilers = new();
        public readonly List<INamedTypeSymbol> Linkers = new();
    }

    internal sealed class SourceGenerationSpec
    {
        public ProjectGeneratorSpec Vcxproj { get; init; }
    }
}
