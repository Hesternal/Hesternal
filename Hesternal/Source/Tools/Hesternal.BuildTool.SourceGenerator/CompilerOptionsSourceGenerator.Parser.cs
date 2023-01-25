using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.Text;

namespace Hesternal.BuildTool.SourceGenerator;


internal sealed partial class CompilerOptionsSourceGenerator : IIncrementalGenerator
{
    private struct Parser
    {
        private readonly SourceProductionContext m_context;
        private readonly SourceText m_compilerOptionsText;


        public static OptionsSpec? Parse(SourceProductionContext context, SourceText compilerOptionsText)
        {
            return new Parser(context, compilerOptionsText)._Parse();
        }


        private Parser(SourceProductionContext context, SourceText compilerOptionsText)
        {
            m_context = context;
            m_compilerOptionsText = compilerOptionsText;
        }


        private OptionsSpec? _Parse()
        {
            // https://github.com/dotnet/roslyn/blob/main/docs/analyzers/Using%20Additional%20Files.md#converting-a-file-to-a-stream
            MemoryStream stream = new();
            using (StreamWriter writer = new(stream, Encoding.UTF8, 1024, true))
            {
                m_compilerOptionsText.Write(writer);
            }
            stream.Position = 0;

            XmlDocument xmlDocument = new();
            xmlDocument.Load(stream);

            XmlNode? mainNode = xmlDocument.ChildNodes[1];

            OptionsSpec? optionsSpec = _ParseMainNode(mainNode);
            if (optionsSpec is not null)
            {
                _ParseCompilerFlags(mainNode, optionsSpec.Flags);

                if (optionsSpec.Flags.Count != 0)
                {
                    return optionsSpec;
                }
            }

            return null;
        }


        private OptionsSpec? _ParseMainNode(XmlNode node)
        {
            string nodeName = node.Name;

            OptionsType optionsType;
            switch (nodeName)
            {
                case "Compiler":
                    optionsType = OptionsType.Compiler;
                    break;
                case "Linker":
                    optionsType = OptionsType.Linker;
                    break;
                default:
                    m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_NodeType, null, nodeName));
                    return null;
            }

            XmlAttributeCollection? nodeAttributes = node.Attributes;
            int numAttributes = nodeAttributes.Count;

            string? name = null;

            for (int i = 0; i < numAttributes; i++)
            {
                XmlAttribute attribute = nodeAttributes[i];
                string attributeName = attribute.Name;

                switch (attributeName)
                {
                    case "Name":
                        name = attribute.Value;
                        break;
                }
            }

            bool criticalError = false;

            if (name is null)
            {
                m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.Invalid_Main_AttributeName, null));
                criticalError = true;
            }

            if (criticalError)
            {
                return null;
            }

            if (optionsType == OptionsType.Compiler)
            {
                return OptionsSpec.Compiler(name!);
            }
            return OptionsSpec.Linker(name!);
        }

        private void _ParseCompilerFlags(XmlNode compilerNode, List<CompilerFlagSpec> compilerFlags)
        {
            XmlNodeList compilerFlagNodes = compilerNode.ChildNodes;
            int numNodes = compilerFlagNodes.Count;

            for (int i = 0; i < numNodes; i++)
            {
                m_context.CancellationToken.ThrowIfCancellationRequested();

                XmlNode compilerFlagNode = compilerFlagNodes[i];
                string nodeName = compilerFlagNode.Name;

                CompilerFlagSpec? compilerFlag;

                switch (nodeName)
                {
                    case "BoolFlag":
                        compilerFlag = _ParseBoolFlag(compilerFlagNode);
                        break;
                    case "EnumFlag":
                        compilerFlag = _ParseEnumFlag(compilerFlagNode);
                        break;
                    case "StringFlag":
                        compilerFlag = _ParseStringFlag(compilerFlagNode);
                        break;
                    case "#comment":
                        continue;
                    default:
                        m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_NodeType, null, nodeName));
                        continue;
                }

                if (compilerFlag is not null)
                {
                    compilerFlags.Add(compilerFlag);
                }
            }
        }

        private CompilerFlagSpec? _ParseBoolFlag(XmlNode boolFlagNode)
        {
            XmlAttributeCollection? nodeAttributes = boolFlagNode.Attributes;
            int numAttributes = nodeAttributes.Count;

            string? name = null;
            string? vcxprojName = null;
            string? flag = null;
            string? reverseFlag = null;
            string? @default = null;
            string? description = null;

            for (int i = 0; i < numAttributes; i++)
            {
                XmlAttribute attribute = nodeAttributes[i];
                string attributeName = attribute.Name;

                switch (attributeName)
                {
                    case "Name":
                        name = attribute.Value;
                        break;
                    case "VcxprojName":
                        vcxprojName = attribute.Value;
                        break;
                    case "Flag":
                        flag = attribute.Value;
                        break;
                    case "ReverseFlag":
                        reverseFlag = attribute.Value;
                        break;
                    case "Default":
                        @default = attribute.Value;
                        break;
                    case "Description":
                        description = attribute.Value;
                        break;
                    default:
                        m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_BoolFlag_InvalidAttribute, null, name ?? null, attributeName));
                        continue;
                }
            }

            bool criticalError = false;

            if (name is null)
            {
                m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_BoolFlag_AttributeName, null));
                criticalError = true;
            }

            bool nullable = false; // NOTE(v.matushkin): Assigning false just to please the compiler

            if (@default is not null)
            {
                if (@default is "true" or "false")
                {
                    nullable = false;
                }
                else
                {
                    m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_BoolFlag_InvalidAttributeDefault, null, name ?? null, @default));
                    criticalError = true;
                }
            }
            else
            {
                nullable = true;
            }

            if (criticalError)
            {
                return null;
            }

            vcxprojName ??= name;

            return new BoolFlagSpec(nullable, name!, vcxprojName, @default, flag, reverseFlag, description);
        }

        private CompilerFlagSpec? _ParseEnumFlag(XmlNode enumFlagNode)
        {
            XmlAttributeCollection? nodeAttributes = enumFlagNode.Attributes;
            int numAttributes = nodeAttributes.Count;

            string? name = null;
            string? vcxprojName = null;
            string? @default = null;
            string? description = null;

            for (int i = 0; i < numAttributes; i++)
            {
                XmlAttribute attribute = nodeAttributes[i];
                string attributeName = attribute.Name;

                switch (attributeName)
                {
                    case "Name":
                        name = attribute.Value;
                        break;
                    case "VcxprojName":
                        vcxprojName = attribute.Value;
                        break;
                    case "Default":
                        @default = attribute.Value;
                        break;
                    case "Description":
                        description = attribute.Value;
                        break;
                    default:
                        m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_EnumFlag_InvalidAttribute, null, name ?? null, attributeName));
                        continue;
                }
            }

            bool criticalError = false;

            if (name is null)
            {
                m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_EnumFlag_AttributeName, null));
                criticalError = true;
            }

            List<EnumFlagSpec.Value> enumValues = _ParseEnumValues(enumFlagNode);
            bool nullable = false; // NOTE(v.matushkin): Assigning false just to please the compiler

            if (@default is not null)
            {
                if (enumValues.FindIndex(value => value.Name == @default) != -1)
                {
                    nullable = false;
                }
                else
                {
                    m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_EnumFlag_InvalidAttributeDefault, null, name ?? null, @default));
                    criticalError = true;
                }
            }
            else
            {
                nullable = true;
            }

            if (criticalError)
            {
                return null;
            }

            vcxprojName ??= name;

            return new EnumFlagSpec(nullable, name!, vcxprojName, @default, enumValues);
        }

        private List<EnumFlagSpec.Value> _ParseEnumValues(XmlNode enumFlagNode)
        {
            List<EnumFlagSpec.Value> enumValues = new();

            XmlNodeList enumValueNodes = enumFlagNode.ChildNodes;
            int numNodes = enumValueNodes.Count;

            for (int i = 0; i < numNodes; i++)
            {
                XmlNode enumValueNode = enumValueNodes[i];
                string enumValueNodeName = enumValueNode.Name;

                if (enumValueNodeName == "#comment")
                {
                    continue;
                }
                if (enumValueNodeName != "EnumValue")
                {
                    m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_EnumFlag_InvalidNode, null, enumValueNodeName));
                    continue;
                }

                string? name = null;
                string? vcxprojName = null;
                string? flag = null;
                string? description = null;

                XmlAttributeCollection? nodeAttributes = enumValueNode.Attributes;
                int numAttributes = nodeAttributes.Count;

                for (int j = 0; j < numAttributes; j++)
                {
                    XmlAttribute attribute = nodeAttributes[j];
                    string attributeName = attribute.Name;

                    switch (attributeName)
                    {
                        case "Name":
                            name = attribute.Value;
                            break;
                        case "VcxprojName":
                            vcxprojName = attribute.Value;
                            break;
                        case "Flag":
                            flag = attribute.Value;
                            break;
                        case "Description":
                            description = attribute.Value;
                            break;
                        default:
                            m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_EnumValue_InvalidAttribute, null, name ?? "null", attributeName));
                            continue;
                    }
                }

                if (name is null)
                {
                    m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_EnumValue_AttributeName, null));
                }
                else
                {
                    vcxprojName ??= name;
                    enumValues.Add(new EnumFlagSpec.Value(name, vcxprojName, flag, description));
                }
            }

            return enumValues;
        }

        private CompilerFlagSpec? _ParseStringFlag(XmlNode stringFlagNode)
        {
            XmlAttributeCollection? nodeAttributes = stringFlagNode.Attributes;
            int numAttributes = nodeAttributes.Count;

            string? name = null;
            string? vcxprojName = null;
            string? flag = null;
            string? description = null;

            for (int i = 0; i < numAttributes; i++)
            {
                XmlAttribute attribute = nodeAttributes[i];
                string attributeName = attribute.Name;

                switch (attributeName)
                {
                    case "Name":
                        name = attribute.Value;
                        break;
                    case "VcxprojName":
                        vcxprojName = attribute.Value;
                        break;
                    case "Flag":
                        flag = attribute.Value;
                        break;
                    case "Description":
                        description = attribute.Value;
                        break;
                    default:
                        m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_StringFlag_InvalidAttribute, null, name ?? null, attributeName));
                        continue;
                }
            }

            if (name is null)
            {
                m_context.ReportDiagnostic(Diagnostic.Create(HesternalDiagnosticDescriptors.InvalidXml_StringFlag_AttributeName, null));
                return null;
            }

            vcxprojName ??= name;

            return new StringFlagSpec(name!, vcxprojName, flag, description);
        }
    }
}
