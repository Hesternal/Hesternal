using System.Text;
using System.Threading;

using Microsoft.CodeAnalysis;

namespace Hesternal.BuildTool.SourceGenerator;


internal sealed partial class CompilerFlagSourceGenerator
{
    private sealed class Emitter
    {
        private static class Indentation
        {
            private const string k_OneLevel = "    ";

            public const string Class = k_OneLevel;
            public const string Method = Class + k_OneLevel;
            public const string Block = Method + k_OneLevel;
        }


        private const int k_StringBuilderInitialCapacity = 500;

        private readonly SourceProductionContext m_context;
        private readonly EnumFlagSpec m_generationSpec;
        private readonly StringBuilder m_sb = new(k_StringBuilderInitialCapacity);


        public static void Emit(SourceProductionContext context, EnumFlagSpec generationSpec)
        {
            new Emitter(context, generationSpec)._Emit();
        }


        private Emitter(SourceProductionContext context, EnumFlagSpec generationSpec)
        {
            m_context = context;
            m_generationSpec = generationSpec;
        }


        private void _Emit()
        {
            Interlocked.Increment(ref s_EmitCount);

            m_sb.AppendLine($"// Init: {s_InitCount}")
                .AppendLine($"// Emit: {s_EmitCount}")
                .AppendLine("using " + HesternalInfo.System.InvalidEnumArgumentException.Namespace + ";")
                .AppendLine()
                .AppendLine("namespace " + HesternalInfo.Namespace.BuildTool + ";")
                .AppendLine()
                .AppendLine()
                .Append("public static class ").Append(m_generationSpec.EnumName).AppendLine("Extensions")
                .AppendLine("{");

            _GenerateMethod_ToVcxprojName();
            m_sb.AppendLine();
            _GenerateMethod_ToCompilerFlag();

            m_sb.AppendLine("}");

            string sourceFileName = m_generationSpec.EnumName + "Extensions.g.cs";
            m_context.AddSource(sourceFileName, m_sb.ToString());
        }


        private void _GenerateMethod_ToVcxprojName()
        {
            m_sb.Append(Indentation.Class + "public static string " + HesternalInfo.GeneratedMethod.CompilerFlag.EnumOption.ToVcxprojName + "(this ")
                    .Append(m_generationSpec.EnumFullyQualifiedName).AppendLine(" value)")
                .AppendLine(Indentation.Class + "{")
                .AppendLine(Indentation.Method + "return value switch")
                .AppendLine(Indentation.Method + "{");

            foreach (EnumFlagOptionSpec option in m_generationSpec.Members)
            {
                m_sb.Append(Indentation.Block).Append(m_generationSpec.EnumFullyQualifiedName).Append('.').Append(option.Name)
                        .Append(" => \"").Append(option.VcxprojName).AppendLine("\",");
            }

            m_sb.AppendLine(Indentation.Block + "_ => throw new " + HesternalInfo.System.InvalidEnumArgumentException.Name + "(),");

            m_sb.AppendLine(Indentation.Method + "};")
                .AppendLine(Indentation.Class + "}");
        }

        private void _GenerateMethod_ToCompilerFlag()
        {
            m_sb.Append(Indentation.Class + "public static string " + HesternalInfo.GeneratedMethod.CompilerFlag.EnumOption.ToCompilerFlag + "(this ")
                    .Append(m_generationSpec.EnumFullyQualifiedName).AppendLine(" value)")
                .AppendLine(Indentation.Class + "{")
                .AppendLine(Indentation.Method + "return value switch")
                .AppendLine(Indentation.Method + "{");

            foreach (EnumFlagOptionSpec option in m_generationSpec.Members)
            {
                m_sb.Append(Indentation.Block).Append(m_generationSpec.EnumFullyQualifiedName).Append('.').Append(option.Name).Append(" => ");
                if (option.Flag is null)
                {
                    m_sb.Append("string.Empty");
                }
                else
                {
                    m_sb.Append('"').Append(option.Flag).Append('"');
                }
                m_sb.AppendLine(",");
            }

            m_sb.AppendLine(Indentation.Block + "_ => throw new " + HesternalInfo.System.InvalidEnumArgumentException.Name + "(),");

            m_sb.AppendLine(Indentation.Method + "};")
                .AppendLine(Indentation.Class + "}");
        }
    }
}
