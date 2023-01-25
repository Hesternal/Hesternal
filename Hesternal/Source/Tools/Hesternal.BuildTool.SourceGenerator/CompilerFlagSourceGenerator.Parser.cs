/*
using System.Collections.Immutable;
using System.Linq;

using Microsoft.CodeAnalysis;

namespace Hesternal.BuildTool.SourceGenerator;


internal sealed partial class CompilerFlagSourceGenerator
{
    private sealed class Parser
    {
        public static EnumFlagSpec GetGenerationSpec(INamedTypeSymbol compilerFlagEnum)
        {
            EnumFlagSpec enumFlagSpec = new()
            {
                EnumName = compilerFlagEnum.Name,
                EnumFullyQualifiedName = compilerFlagEnum.ToFullyQualifiedName(),
            };

            foreach (ISymbol enumMemberSymbol in compilerFlagEnum.GetMembers())
            {
                if (enumMemberSymbol is IFieldSymbol)
                {
                    enumFlagSpec.Members.Add(_GetEnumOptionSpec(enumMemberSymbol));
                }
            }

            return enumFlagSpec;
        }


        private static EnumFlagOptionSpec _GetEnumOptionSpec(ISymbol enumMemberSymbol)
        {
            AttributeData? enumOptionAttributeData = enumMemberSymbol.GetAttributes()
                .FirstOrDefault(static attributeData => attributeData.AttributeClass?.Name == HesternalInfo.Attribute.CompilerFlag.EnumOption.Name);

            string? flag;
            string vcxprojName;

            if (enumOptionAttributeData is null)
            {
                flag = null;
                vcxprojName = enumMemberSymbol.Name;
            }
            else
            {
                ImmutableArray<TypedConstant> arguments = enumOptionAttributeData.ConstructorArguments;

                if (arguments.Length != HesternalInfo.Attribute.CompilerFlag.EnumOption.NumConstructorArguments)
                {
                    // TODO: REPORT
                }

                {
                    TypedConstant flagArgument = arguments[HesternalInfo.Attribute.CompilerFlag.EnumOption.FlagArgumentIndex];
                    flag = (string?)flagArgument.Value;
                }

                {
                    TypedConstant vcxprojNameArgument = arguments[HesternalInfo.Attribute.CompilerFlag.EnumOption.VcxprojNameArgumentIndex];
                    vcxprojName = vcxprojNameArgument.IsNull
                                ? enumMemberSymbol.Name
                                : (string)vcxprojNameArgument.Value!;
                }
            }

            return new EnumFlagOptionSpec
            {
                Name = enumMemberSymbol.Name,
                Flag = flag,
                VcxprojName = vcxprojName,
            };
        }
    }
}
*/
