using System.Collections.Generic;
using System.Text;
using System.Threading;

using Microsoft.CodeAnalysis;

namespace Hesternal.BuildTool.SourceGenerator;


internal sealed partial class ProjectSourceGenerator
{
    private sealed class Emitter
    {
        private enum FieldType
        {
            NotSupported,
            Bool,
            Enum,
            String,
        }

        private static class Indentation
        {
            private const string k_OneLevel = "    ";

            public const string Class  = k_OneLevel;
            public const string Method = Class + k_OneLevel;
            public const string If     = Method + k_OneLevel;
        }

        private static class MethodName
        {
            public const string Compiler = "_WriteCompilerOptions";
            public const string Linker   = "_WriteLinkerOptions";
        }

        private static class OptionsInterface
        {
            // TODO(v.matushkin): <Hardcoded/ProjectName>
            private const string k_Namespace = "global::Hesternal.BuildTool.";

            public const string BuildSystem = k_Namespace + "IBuildSystemOptions";
            public const string Compiler    = k_Namespace + "ICompilerOptions";
            public const string Linker      = k_Namespace + "ILinkerOptions";
        }


        private const int k_StringBuilderInitialCapacity = 9500;

        private const string k_GeneratedSourceHeader =
@"using System;

namespace Hesternal.BuildTool.Generators.VisualStudio;


internal sealed partial class VsCppProjectGenerator
{";

        private readonly SourceProductionContext m_context;
        private readonly SourceGenerationSpec m_generationSpec;
        private readonly StringBuilder m_sb = new(k_StringBuilderInitialCapacity);


        public static void Emit(SourceProductionContext context, SourceGenerationSpec generationSpec)
        {
            new Emitter(context, generationSpec)._Emit();
        }


        private Emitter(SourceProductionContext context, SourceGenerationSpec generationSpec)
        {
            m_context = context;
            m_generationSpec = generationSpec;
        }


        private void _Emit()
        {
            Interlocked.Increment(ref s_EmitCount);

            _AddSource("Compiler", _EmitOptionsSource(MethodName.Compiler, OptionsInterface.Compiler, m_generationSpec.Vcxproj.Compilers, true));
            m_sb.Clear();
            _AddSource("Linker", _EmitOptionsSource(MethodName.Linker, OptionsInterface.Linker, m_generationSpec.Vcxproj.Linkers));

            void _AddSource(string name, string code)
            {
                string sourceFileName = $"VsCppProjectGenerator.{name}.g.cs";
                m_context.AddSource(sourceFileName, code);
            }
        }

        private string _EmitOptionsSource(string methodName, string optionsInterface, List<INamedTypeSymbol> optionsTypes, bool writeStats = false)
        {
            if (writeStats)
            {
                m_sb.AppendLine($"// Init: {s_InitCount}")
                    .AppendLine($"// Select: {s_SelectCount}")
                    .AppendLine($"// Emit: {s_EmitCount}");
            }

            m_sb.AppendLine(k_GeneratedSourceHeader)
                .Append(Indentation.Class + "private void _WriteOptions(").Append(optionsInterface).AppendLine(" options)")
                .AppendLine(Indentation.Class + "{")
                .Append(Indentation.Method);

            _WriteIfBlock(optionsTypes[0]);

            for (int i = 1; i < optionsTypes.Count; i++)
            {
                m_sb.Append(Indentation.Method + "else ");
                _WriteIfBlock(optionsTypes[i]);
            }

            m_sb.AppendLine(Indentation.Method + "else")
                .AppendLine(Indentation.Method + "{")
                .AppendLine(Indentation.If + "throw new ArgumentException();")
                .AppendLine(Indentation.Method + "}")
                .AppendLine(Indentation.Class + "}");

            foreach (INamedTypeSymbol options in optionsTypes)
            {
                _GenerateOptionsMethod(options, methodName);
            }

            m_sb.AppendLine("}");

            return m_sb.ToString();

            void _WriteIfBlock(INamedTypeSymbol type)
            {
                string typeNameVariable = type.Name.ToLower();
                string typeFullName = type.ToDisplayString(SymbolDisplayFormat.FullyQualifiedFormat);

                m_sb.Append("if (options is ").Append(typeFullName).Append(' ').Append(typeNameVariable).AppendLine(")")
                    .AppendLine(Indentation.Method + "{")
                    .Append(Indentation.If).Append(methodName).Append('(').Append(typeNameVariable).AppendLine(");")
                    .AppendLine(Indentation.Method + "}");
            }
        }


        private void _GenerateOptionsMethod(INamedTypeSymbol memberTypeSymbol, string methodName)
        {
            string typeFullName = memberTypeSymbol.ToDisplayString(SymbolDisplayFormat.FullyQualifiedFormat);

            m_sb.Append(Indentation.Class + "private void ").Append(methodName).Append('(').Append(typeFullName).AppendLine(" options)")
                .AppendLine(Indentation.Class + "{");

            foreach (ISymbol memberSymbol in memberTypeSymbol.GetMembers())
            {
                // NOTE(v.matushkin): memberSymbol.Kind == SymbolKind.Field ?
                if (memberSymbol is IFieldSymbol fieldSymbol and { DeclaredAccessibility: Accessibility.Public })
                {
                    _GenerateField(fieldSymbol);
                }
            }

            m_sb.AppendLine(Indentation.Class + "}");
        }

        private void _GenerateField(IFieldSymbol fieldSymbol)
        {
            (bool isNullable, FieldType fieldType) = _GetFieldType((INamedTypeSymbol)fieldSymbol.Type);

            if (fieldType == FieldType.NotSupported)
            {
                //context.ReportDiagnostic(_Diagnostic($"[Member Field] UNHANDLED! Name: {fieldSymbol.Name} | SpecialType: {fieldSymbol.Type.SpecialType}"));
                return;
            }

            bool needCheck = isNullable || fieldType == FieldType.String;

            m_sb.Append(Indentation.Method);

            if (needCheck)
            {
                m_sb.Append("if (options.").Append(fieldSymbol.Name);

                if (isNullable)
                {
                    m_sb.AppendLine(".HasValue)");
                }
                else
                {
                    m_sb.AppendLine(" is not null)");
                }

                m_sb.AppendLine(Indentation.Method + "{")
                    .Append(Indentation.If);
            }

            m_sb.Append("m_projectWriter.Property(\"").Append(fieldSymbol.Name).Append("\", options.").Append(fieldSymbol.Name);

            if (isNullable)
            {
                m_sb.Append(".Value");
            }

            if (fieldType == FieldType.Enum)
            {
                m_sb.Append(".ToString()");
            }

            m_sb.AppendLine(");");

            if (needCheck)
            {
                m_sb.AppendLine(Indentation.Method + "}");
            }
        }


        private static (bool isNullable, FieldType) _GetFieldType(INamedTypeSymbol fieldTypeSymbol)
        {
            bool isNullable = false;
            FieldType fieldType = FieldType.NotSupported;

            if (fieldTypeSymbol is { IsDefinition: false, OriginalDefinition.SpecialType: SpecialType.System_Nullable_T })
            {
                isNullable = true;
                fieldTypeSymbol = (INamedTypeSymbol)fieldTypeSymbol.TypeArguments[0];
            }

            if (fieldTypeSymbol.SpecialType == SpecialType.System_Boolean)
            {
                fieldType = FieldType.Bool;
            }
            else if (fieldTypeSymbol.SpecialType == SpecialType.System_String)
            {
                fieldType = FieldType.String;
            }
            else if (fieldTypeSymbol.TypeKind == TypeKind.Enum)
            {
                fieldType = FieldType.Enum;
            }

            return (isNullable, fieldType);
        }
    }
}
