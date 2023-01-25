using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;

using Microsoft.CodeAnalysis;

namespace Hesternal.BuildTool.SourceGenerator;


internal sealed partial class ProjectSourceGenerator
{
    private sealed class Emitter : IDisposable
    {
        private enum FieldType
        {
            NotSupported,
            Bool,
            Enum,
            String,
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


        private const int k_InitialCapacity = 9500;

        private readonly SourceProductionContext m_context;
        private readonly SourceGenerationSpec m_generationSpec;

        private readonly MemoryStream m_writerStream;
        private readonly CSharpWriter m_writer;


        public static void Emit(SourceProductionContext context, SourceGenerationSpec generationSpec)
        {
            using Emitter emitter = new(context, generationSpec);
            emitter._Emit();
        }


        private Emitter(SourceProductionContext context, SourceGenerationSpec generationSpec)
        {
            m_context = context;
            m_generationSpec = generationSpec;

            m_writerStream = new MemoryStream(k_InitialCapacity);
            m_writer = new CSharpWriter(m_writerStream);
        }

        public void Dispose() => m_writer.Dispose();


        private void _Emit()
        {
            Interlocked.Increment(ref s_EmitCount);

            _AddSource("Compiler", _EmitOptionsSource(MethodName.Compiler, OptionsInterface.Compiler, m_generationSpec.Vcxproj.Compilers, true));
            m_writerStream.SetLength(0);
            _AddSource("Linker", _EmitOptionsSource(MethodName.Linker, OptionsInterface.Linker, m_generationSpec.Vcxproj.Linkers));

            m_writer.Dispose();

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
                m_writer.WriteLine("// Init: {0}", s_InitCount);
                m_writer.WriteLine("// Select: {0}", s_SelectCount);
                m_writer.WriteLine("// Emit: {0}", s_EmitCount);
            }

            m_writer.WriteUsing("System");
            m_writer.WriteLine();
            m_writer.WriteNamespace("Hesternal.BuildTool.Generators.VisualStudio");
            m_writer.WriteLines(2);
            m_writer.WriteStartClass("VsCppProjectGenerator", CSharpWriter.Visibility.Internal, CSharpWriter.ClassType.SealedPartial);

            m_writer.WriteLine("private void _WriteOptions({0} options)", optionsInterface);
            m_writer.WriteStartBlock();

            _WriteIfBlock(optionsTypes[0]);

            for (int i = 1; i < optionsTypes.Count; i++)
            {
                m_writer.Write("else ");
                _WriteIfBlock(optionsTypes[i]);
            }

            m_writer.WriteLine("else");
            m_writer.WriteStartBlock();
            m_writer.WriteLine("throw new ArgumentException();");
            m_writer.WriteEndBlock();

            m_writer.WriteEndBlock(); // method

            foreach (INamedTypeSymbol options in optionsTypes)
            {
                _GenerateOptionsMethod(options, methodName);
            }

            m_writer.WriteEndClass();

            m_writer.Flush();

            return m_writerStream.ToUtf8String();

            void _WriteIfBlock(INamedTypeSymbol type)
            {
                string typeNameVariable = type.Name.ToLower();
                string typeFullName = type.ToFullyQualifiedName();

                m_writer.WriteLine("if (options is {0} {1})", typeFullName, typeNameVariable);
                m_writer.WriteStartBlock();
                m_writer.WriteLine("{0}({1});", methodName, typeNameVariable);
                m_writer.WriteEndBlock();
            }
        }


        private void _GenerateOptionsMethod(INamedTypeSymbol memberTypeSymbol, string methodName)
        {
            string typeFullName = memberTypeSymbol.ToDisplayString(SymbolDisplayFormat.FullyQualifiedFormat);

            m_writer.WriteLine("private void {0}({1} options)", methodName, typeFullName);
            m_writer.WriteStartBlock();

            foreach (ISymbol memberSymbol in memberTypeSymbol.GetMembers())
            {
                // NOTE(v.matushkin): memberSymbol.Kind == SymbolKind.Field ?
                if (memberSymbol is IFieldSymbol fieldSymbol and { DeclaredAccessibility: Accessibility.Public })
                {
                    _GenerateField(fieldSymbol);
                }
            }

            m_writer.WriteEndBlock();
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

            if (needCheck)
            {
                m_writer.Write("if (options.{0}", fieldSymbol.Name);
                if (isNullable)
                {
                    m_writer.Write(".HasValue");
                }
                else
                {
                    m_writer.Write(" is not null");
                }
                m_writer.WriteLine(')');
                m_writer.WriteStartBlock();
            }

            m_writer.Write("m_projectWriter.Property(\"{0}\", options.{0}", fieldSymbol.Name);

            if (isNullable)
            {
                m_writer.Write(".Value");
            }

            if (fieldType == FieldType.Enum)
            {
                m_writer.Write("." + HesternalInfo.GeneratedMethod.CompilerFlag.EnumOption.ToVcxprojName + "()");
            }

            m_writer.WriteLine(");");

            if (needCheck)
            {
                m_writer.WriteEndBlock();
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
