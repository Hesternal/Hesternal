using System.Collections.Generic;
using System.Collections.Immutable;

using Microsoft.CodeAnalysis;

namespace Hesternal.BuildTool.SourceGenerator;


internal sealed partial class ProjectSourceGenerator
{
    private static class ProjectGeneratorName
    {
        // TODO(v.matushkin): <Hardcoded/ProjectName>
        public const string Vcxproj = "Hesternal.BuildTool.Generators.VisualStudio.VsCppProjectGenerator";
    }


    private sealed class Parser
    {
        private static class AttributeName
        {
            public const string SupportedBuildSystems = "SupportedBuildSystemsAttribute";
            public const string SupportedCompilers    = "SupportedCompilersAttribute";
            public const string SupportedLinkers      = "SupportedLinkersAttribute";
        }


        public static SourceGenerationSpec GetGenerationSpec(ImmutableArray<INamedTypeSymbol> projectGenerators)
        {
            ProjectGeneratorSpec vcxproj = null!;

            foreach (INamedTypeSymbol projectGenerator in projectGenerators)
            {
                if (projectGenerator.Name == "VsCppProjectGenerator") //ProjectGeneratorName.Vcxproj)
                {
                    vcxproj = _GetProjectGeneratorSpec(projectGenerator);
                    break;
                }
            }

            return new SourceGenerationSpec()
            {
                Vcxproj = vcxproj!,
            };
        }


        private static ProjectGeneratorSpec _GetProjectGeneratorSpec(INamedTypeSymbol projectGenerator)
        {
            ProjectGeneratorSpec spec = new();

            foreach (AttributeData attributeData in projectGenerator.GetAttributes())
            {
                if (attributeData.AttributeClass is not null)
                {
                    string attributeName = attributeData.AttributeClass.Name;

                    if (attributeName == AttributeName.SupportedBuildSystems)
                    {
                        _AddAttributeConstructorTypeArguments(spec.BuildSystems, attributeData);
                    }
                    else if (attributeName == AttributeName.SupportedCompilers)
                    {
                        _AddAttributeConstructorTypeArguments(spec.Compilers, attributeData);
                    }
                }
            }

            _AddSupportedLinkers(spec.Compilers, spec.Linkers);

            return spec;
        }


        private static void _AddSupportedLinkers(List<INamedTypeSymbol> compilers, List<INamedTypeSymbol> linkers)
        {
            foreach (INamedTypeSymbol compiler in compilers)
            {
                foreach (AttributeData attributeData in compiler.GetAttributes())
                {
                    if (attributeData.AttributeClass is not null)
                    {
                        string attributeName = attributeData.AttributeClass.Name;

                        if (attributeName == AttributeName.SupportedLinkers)
                        {
                            _AddAttributeConstructorTypeArguments(linkers, attributeData);
                        }
                    }
                }
            }
        }


        private static void _AddAttributeConstructorTypeArguments(List<INamedTypeSymbol> dst, AttributeData attributeData)
        {
            ImmutableArray<TypedConstant> arguments = attributeData.ConstructorArguments;

            if (arguments.Length != 1 || arguments[0].Kind != TypedConstantKind.Array)
            {
                return;
            }

            foreach (TypedConstant typeArgument in arguments[0].Values)
            {
                if (typeArgument.Value is not null)
                {
                    dst.Add((INamedTypeSymbol)typeArgument.Value);
                }
            }
        }
    }
}
