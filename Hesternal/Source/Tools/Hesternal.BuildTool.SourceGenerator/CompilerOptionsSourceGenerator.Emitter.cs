using System;
using System.ComponentModel;
using System.IO;
using System.Threading;

using Microsoft.CodeAnalysis;

namespace Hesternal.BuildTool.SourceGenerator;


internal sealed partial class CompilerOptionsSourceGenerator : IIncrementalGenerator
{
    private sealed class Emitter : IDisposable
    {
        private const string k_Namespace = "Hesternal.BuildTool";

        private const string k_ICompilerOptions = "ICompilerOptions";
        private const string k_ILinkerOptions = "ILinkerOptions";
        // private const string k_BoolAttribute = "CompilerFlag.Bool";
        // private const string k_EnumOptionAttribute = "CompilerFlag.EnumOption";
        // private const string k_StringAttribute = "CompilerFlag.String";

        private const int k_InitialCapacity = 500;

        private readonly SourceProductionContext m_context;
        private readonly OptionsSpec m_spec;

        private readonly string m_optionsName;

        private readonly MemoryStream m_optionsStream;
        private readonly MemoryStream m_wtfStream;
        private readonly MemoryStream m_descStream;
        private readonly MemoryStream m_enumsStream;
        // private readonly MemoryStream m_enumsExtensionsStream;
        private readonly CSharpWriter m_optionsWriter;
        private readonly CSharpWriter m_wtfWriter;
        private readonly CSharpWriter m_descWriter;
        private readonly CSharpWriter m_enumsWriter;
        // private readonly CSharpWriter m_enumsExtensionsWriter;
        private bool m_hasEnums = false;


        public static void Emit(SourceProductionContext context, OptionsSpec compilerOptionsSpec)
        {
            using Emitter emitter = new(context, compilerOptionsSpec);
            emitter._Emit();
        }


        private Emitter(SourceProductionContext context, OptionsSpec compilerOptionsSpec)
        {
            m_context = context;
            m_spec = compilerOptionsSpec;

            m_optionsName = compilerOptionsSpec.Name + "Options";

            m_optionsStream = new MemoryStream(k_InitialCapacity);
            m_wtfStream = new MemoryStream(k_InitialCapacity);
            m_descStream = new MemoryStream(k_InitialCapacity);
            m_enumsStream = new MemoryStream(k_InitialCapacity);
            // m_enumsExtensionsStream = new MemoryStream(k_InitialCapacity);
            m_optionsWriter = new CSharpWriter(m_optionsStream);
            m_wtfWriter = new CSharpWriter(m_wtfStream);
            m_descWriter = new CSharpWriter(m_descStream);
            m_enumsWriter = new CSharpWriter(m_enumsStream);
            // m_enumsExtensionsWriter = new CSharpWriter(m_enumsExtensionsStream);
        }

        public void Dispose()
        {
            m_optionsWriter.Dispose();
            m_descWriter.Dispose();
            m_enumsWriter.Dispose();
            // m_enumsExtensionsWriter.Dispose();
        }


        private void _Emit()
        {
            Interlocked.Increment(ref s_EmitCount);

            _Start();
            _WriteOptions();
            _End();
        }

        private void _Start()
        {
            //- Options
            m_optionsWriter.WriteLine("// Init: {0}", s_InitCount);
            m_optionsWriter.WriteLine("// Emit: {0}", s_EmitCount);

            m_optionsWriter.WriteStartUsingStatic();
            m_optionsWriter.Write(k_Namespace + ".{0}", m_spec.Name);
            m_optionsWriter.WriteEndUsingStatic();
            m_optionsWriter.WriteLine();

            m_optionsWriter.WriteNamespace(k_Namespace);
            m_optionsWriter.WriteLine();
            m_optionsWriter.WriteNullableEnable();
            m_optionsWriter.WriteLines(2);
            m_optionsWriter.WriteStartClass(m_optionsName, CSharpWriter.Visibility.Public, CSharpWriter.ClassType.SealedPartial,
                m_spec.Type == OptionsType.Compiler ? k_ICompilerOptions : k_ILinkerOptions);

            //- Wtf
            m_wtfWriter.WriteNamespace(k_Namespace);
            m_wtfWriter.WriteLines(2);

            m_wtfWriter.WriteStartClass(m_spec.Name, CSharpWriter.Visibility.Public, CSharpWriter.ClassType.StaticPartial);

            //- Desc
            m_descWriter.WriteNamespace(k_Namespace);
            m_descWriter.WriteLines(2);

            m_descWriter.WriteStartClass(m_spec.Name, CSharpWriter.Visibility.Public, CSharpWriter.ClassType.StaticPartial);

            //- Enums
            m_enumsWriter.WriteNamespace(k_Namespace);
            m_enumsWriter.WriteLine();
            m_enumsWriter.WriteNullableEnable();
            m_enumsWriter.WriteLines(2);

            m_enumsWriter.WriteStartClass(m_spec.Name, CSharpWriter.Visibility.Public, CSharpWriter.ClassType.StaticPartial);

            //- Enums extensions
//            m_enumsExtensionsWriter.WriteUsing(HesternalInfo.System.InvalidEnumArgumentException.Namespace);
//            m_enumsExtensionsWriter.WriteLine();
//            m_enumsExtensionsWriter.WriteStartUsingStatic();
//            m_enumsExtensionsWriter.Write(k_Namespace + ".{0}", m_spec.Name);
//            m_enumsExtensionsWriter.WriteEndUsingStatic();
//            m_enumsExtensionsWriter.WriteLine();
//
//            m_enumsExtensionsWriter.WriteNamespace(k_Namespace);
//            m_enumsExtensionsWriter.WriteLine();
//            m_enumsExtensionsWriter.WriteNullableEnable();
//            m_enumsExtensionsWriter.WriteLines(2);
//
//            m_enumsExtensionsWriter.WriteStartClass(m_optionsName + "Extensions", CSharpWriter.Visibility.Public, CSharpWriter.ClassType.Static);
        }

        private void _End()
        {
            string optionsSourceFileName = m_optionsName + ".g.cs";
            EndWriter(m_context, optionsSourceFileName, m_optionsStream, m_optionsWriter);

            string wtfSourceFileName = m_spec.Name + ".g.cs";
            EndWriter(m_context, wtfSourceFileName, m_wtfStream, m_wtfWriter);

            string descSourceFileName = m_spec.Name + ".Desc.g.cs";
            EndWriter(m_context, descSourceFileName, m_descStream, m_descWriter);

            if (m_hasEnums)
            {
                string enumSourceFileName = m_spec.Name + ".Enum.g.cs";
                EndWriter(m_context, enumSourceFileName, m_enumsStream, m_enumsWriter);

                // string enumExtensionsSourceFileName = m_optionsName + ".EnumExtensions.g.cs";
                // EndWriter(m_context, enumExtensionsSourceFileName, m_enumsExtensionsStream, m_enumsExtensionsWriter);
            }

            static void EndWriter(SourceProductionContext context, string sourceFileName, MemoryStream underlyingStream, CSharpWriter writer)
            {
                writer.WriteEndClass();
                writer.Flush();

                string code = underlyingStream.ToUtf8String();
                context.AddSource(sourceFileName, code);

                writer.Dispose();
            }
        }

        private void _WriteOptions()
        {
            bool needNewLine = false;

            foreach (CompilerFlagSpec compilerFlag in m_spec.Flags)
            {
                if (needNewLine)
                {
                    m_optionsWriter.WriteLine();
                }
                else
                {
                    needNewLine = true;
                }

                switch (compilerFlag.Type)
                {
                    case CompilerFlagType.Bool:
                        {
                            BoolFlagSpec boolFlag = (BoolFlagSpec)compilerFlag;
                            _WriteBoolOption(boolFlag);
                            _WriteBoolDesc(boolFlag);
                            _WriteBoolWtf(boolFlag);
                        }
                        break;
                    case CompilerFlagType.Enum:
                        {
                            EnumFlagSpec enumFlag = (EnumFlagSpec)compilerFlag;
                            _WriteEnumOption(enumFlag);
                            _WriteEnum(enumFlag);
                            m_hasEnums = true;
                        }
                        break;
                    case CompilerFlagType.String:
                        {
                            StringFlagSpec stringFlag = (StringFlagSpec)compilerFlag;
                            _WriteStringOption(stringFlag);
                            _WriteStringDesc(stringFlag);
                            _WriteStringWtf(stringFlag);
                        }
                        break;
                    default:
                        throw new InvalidEnumArgumentException();
                }
            }
        }


        private void _WriteBoolOption(BoolFlagSpec boolFlag)
        {
            //- Documentation
            //-- Summary
            if (boolFlag.Description is not null)
            {
                m_optionsWriter.WriteLine("/// <summary>");
                m_optionsWriter.WriteLine("/// {0}", boolFlag.Description);
                m_optionsWriter.WriteLine("/// </summary>");
            }
            //-- Remarks
            bool hasFlag = boolFlag.Flag is not null;
            bool hasReverseFlag = boolFlag.ReverseFlag is not null;
            if (hasFlag || hasReverseFlag)
            {
                m_optionsWriter.WriteLine("/// <remarks>");
                m_optionsWriter.Write("/// ");
                if (hasFlag)
                {
                    m_optionsWriter.Write("<see langword=\"true\"/> {0}", boolFlag.Flag!);
                }
                if (hasReverseFlag)
                {
                    if (hasFlag)
                    {
                        m_optionsWriter.Write(", ");
                    }
                    m_optionsWriter.Write("<see langword=\"false\"/> {0}", boolFlag.ReverseFlag!);
                }
                m_optionsWriter.WriteLine();
                m_optionsWriter.WriteLine("/// </remarks>");
            }

            //- Field
            m_optionsWriter.Write("public bool");
            if (boolFlag.Nullable)
            {
                m_optionsWriter.Write('?');
            }
            m_optionsWriter.WriteLine(" {0} = {1};", boolFlag.Name, boolFlag.Default);
        }

        private void _WriteEnumOption(EnumFlagSpec enumFlag)
        {
            //- Field
            m_optionsWriter.Write(CSharpWriter.Visibility.Public + " {0}", enumFlag.Name);
            if (enumFlag.Nullable)
            {
                m_optionsWriter.Write('?');
            }
            m_optionsWriter.Write(" {0} = ", enumFlag.Name);
            if (enumFlag.Nullable == false)
            {
                m_optionsWriter.Write("{0}.", enumFlag.Name);
            }
            m_optionsWriter.WriteLine("{0};", enumFlag.Default);
        }

        private void _WriteStringOption(StringFlagSpec stringFlag)
        {
            //- Documentation
            //-- Summary
            if (stringFlag.Description is not null)
            {
                m_optionsWriter.WriteLine("/// <summary>");
                m_optionsWriter.WriteLine("/// {0}", stringFlag.Description);
                m_optionsWriter.WriteLine("/// </summary>");
            }
            //-- Remarks
            if (stringFlag.Flag is not null)
            {
                m_optionsWriter.WriteLine("/// <remarks> {0} </remarks>", stringFlag.Flag);
            }

            //- Field
            m_optionsWriter.Write(CSharpWriter.Visibility.Public + " string");
            if (stringFlag.Nullable)
            {
                m_optionsWriter.Write('?');
            }
            m_optionsWriter.WriteLine(" {0} = {1};", stringFlag.Name, stringFlag.Default);
        }


        private void _WriteBoolDesc(BoolFlagSpec boolFlag)
        {
            m_descWriter.Write("private static readonly BoolFlagDesc s_{0} = new(", boolFlag.Name);
            _WriteNullOrQuotedString(m_descWriter, boolFlag.Flag);
            m_descWriter.Write(", ");
            _WriteNullOrQuotedString(m_descWriter, boolFlag.ReverseFlag);
            m_descWriter.Write(", ");
            _WriteNullOrQuotedString(m_descWriter, boolFlag.VcxprojName);
            m_descWriter.WriteLine(");");
        }

        private void _WriteStringDesc(StringFlagSpec stringFlag)
        {
            m_descWriter.Write("private static readonly StringFlagDesc s_{0} = new(", stringFlag.Name);
            _WriteNullOrQuotedString(m_descWriter, stringFlag.Flag);
            m_descWriter.Write(", ");
            _WriteNullOrQuotedString(m_descWriter, stringFlag.VcxprojName);
            m_descWriter.WriteLine(");");
        }


        private void _WriteBoolWtf(BoolFlagSpec boolFlag)
        {
            m_wtfWriter.Write("public static BoolFlag {0}(bool? initialValue = ", boolFlag.Name);
            _WriteNullOrQuotedString(m_wtfWriter, boolFlag.Default);
            m_wtfWriter.WriteLine(") => s_{0}.MakeFlag(initialValue);", boolFlag.Name);
        }

        private void _WriteStringWtf(StringFlagSpec stringFlag)
        {
            m_wtfWriter.Write("public static StringFlag {0}(string? initialValue = ", stringFlag.Name);
            _WriteNullOrQuotedString(m_wtfWriter, stringFlag.Default);
            m_wtfWriter.WriteLine(") => s_{0}.MakeFlag(initialValue);", stringFlag.Name);
        }


        private void _WriteEnum(EnumFlagSpec flag)
        {
            if (m_hasEnums)
            {
                m_enumsWriter.WriteLine();
            }

            //- Documentation
            if (flag.Description is not null)
            {
                m_enumsWriter.WriteLine("/// <summary>");
                m_enumsWriter.WriteLine("/// {0}", flag.Description);
                m_enumsWriter.WriteLine("/// </summary>");
            }

            m_enumsWriter.WriteStartClass(flag.Name, CSharpWriter.Visibility.Public, CSharpWriter.ClassType.Sealed);

            //- Constructor
            m_enumsWriter.WriteLine("private {0}(string? flag, string? vcxprojValue)", flag.Name);
            m_enumsWriter.Write("    : base(flag, ");
            _WriteNullOrQuotedString(m_enumsWriter, flag.VcxprojName);
            m_enumsWriter.WriteLine(", vcxprojValue)");
            m_enumsWriter.WriteStartBlock();
            m_enumsWriter.WriteEndBlock();
            m_enumsWriter.WriteLines(2);

            //- Static "Enum" fields
            foreach (EnumFlagSpec.Value enumValue in flag.Values)
            {
                //-- Documentation
                if (enumValue.Description is not null)
                {
                    m_enumsWriter.WriteLine("/// <summary>");
                    m_enumsWriter.WriteLine("/// {0}", enumValue.Description);
                    m_enumsWriter.WriteLine("/// </summary>");
                }
                if (enumValue.Flag is not null)
                {
                    m_enumsWriter.WriteLine("/// <remarks>{0}</remarks>", enumValue.Flag);
                }

                //-- Enum value
                m_enumsWriter.Write("public static readonly {0} {1} = new(", flag.Name, enumValue.Name);
                _WriteNullOrQuotedString(m_enumsWriter, enumValue.Flag);
                m_enumsWriter.Write(", ");
                _WriteNullOrQuotedString(m_enumsWriter, enumValue.VcxprojName);
                m_enumsWriter.WriteLine(");");
            }

            m_enumsWriter.WriteEndClass();
        }

        /*
        private void _WriteEnumExtensions(EnumFlagSpec flag)
        {
            const string toCompilerFlag = HesternalInfo.GeneratedMethod.CompilerFlag.EnumOption.ToCompilerFlag;
            const string toVcxprojName = HesternalInfo.GeneratedMethod.CompilerFlag.EnumOption.ToVcxprojName;

            if (m_hasEnums)
            {
                m_enumsExtensionsWriter.WriteLine();
            }

            //- ToCompilerFlag
            m_enumsExtensionsWriter.Write("public static string");
            if (flag.HasNullFlags)
            {
                m_enumsExtensionsWriter.Write('?');
            }
            m_enumsExtensionsWriter.WriteLine(" " + toCompilerFlag + "(this {0} value)", flag.Name);
            m_enumsExtensionsWriter.WriteStartBlock();
            m_enumsExtensionsWriter.WriteLine("return value switch");
            m_enumsExtensionsWriter.WriteStartBlock();
            foreach (EnumFlagSpec.Value value in flag.Values)
            {
                m_enumsExtensionsWriter.Write("{0}.{1} => ", flag.Name, value.Name);
                if (value.Flag is null)
                {
                    m_enumsExtensionsWriter.Write("null");
                }
                else
                {
                    m_enumsExtensionsWriter.Write("\"{0}\"", value.Flag);
                }
                m_enumsExtensionsWriter.WriteLine(',');
            }
            m_enumsExtensionsWriter.WriteLine("_ => throw new " + HesternalInfo.System.InvalidEnumArgumentException.Name + "(),");
            m_enumsExtensionsWriter.WriteEndSwitch();
            m_enumsExtensionsWriter.WriteEndBlock(); // method

            //- ToVcxprojName
            m_enumsExtensionsWriter.WriteLine("public static string " + toVcxprojName + "(this {0} value)", flag.Name);
            m_enumsExtensionsWriter.WriteStartBlock();
            m_enumsExtensionsWriter.WriteLine("return value switch");
            m_enumsExtensionsWriter.WriteStartBlock();
            foreach (EnumFlagSpec.Value value in flag.Values)
            {
                m_enumsExtensionsWriter.WriteLine("{0}.{1} => \"{2}\",", flag.Name, value.Name, value.VcxprojName ?? value.Name);
            }
            m_enumsExtensionsWriter.WriteLine("_ => throw new " + HesternalInfo.System.InvalidEnumArgumentException.Name + "(),");
            m_enumsExtensionsWriter.WriteEndSwitch();
            m_enumsExtensionsWriter.WriteEndBlock(); // method
        }
        */


        private static void _WriteNullOrQuotedString(CSharpWriter writer, string? str)
        {
            if (str is null)
            {
                writer.Write("null");
            }
            else
            {
                writer.Write("\"{0}\"", str);
            }
        }
    }
}
