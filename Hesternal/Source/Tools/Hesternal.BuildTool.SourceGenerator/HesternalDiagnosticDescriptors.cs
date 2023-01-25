using Microsoft.CodeAnalysis;

namespace Hesternal.BuildTool.SourceGenerator;


internal static class HesternalDiagnosticDescriptors
{
    public static readonly DiagnosticDescriptor CompilerOptionsMustBePartial
        = new("HS001", "CompilerOptions class must be partial", "The '{0}' must be partial",
            "HesternalAnalyzer", DiagnosticSeverity.Error, true);

    //- CompilerOptions

    private const string k_CompilerOptionsTitle = "Invalid XML";
    private const string k_CompilerOptionsCategory = "Hesternal.CompilerOptions";

    public static readonly DiagnosticDescriptor Invalid_Main_AttributeName
        = new("HS101", k_CompilerOptionsTitle,
            "Attribute 'Name' was not found on the main node",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    public static readonly DiagnosticDescriptor InvalidXml_NodeType
        = new("HS102", k_CompilerOptionsTitle,
            "Invalid node type: '{0}'",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    //-- BoolFlag

    public static readonly DiagnosticDescriptor InvalidXml_BoolFlag_InvalidAttribute
        = new("HS103", k_CompilerOptionsTitle,
            "BoolFlag[{0}] invalid attribute '{1}'",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    public static readonly DiagnosticDescriptor InvalidXml_BoolFlag_AttributeName
        = new("HS104", k_CompilerOptionsTitle,
            "BoolFlag required attribute 'Name' was not found",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    public static readonly DiagnosticDescriptor InvalidXml_BoolFlag_AttributeDefault
        = new("HS105", k_CompilerOptionsTitle,
            "BoolFlag[{0}] required attribute 'Default' was not found",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    public static readonly DiagnosticDescriptor InvalidXml_BoolFlag_InvalidAttributeDefault
        = new("HS106", k_CompilerOptionsTitle,
            "BoolFlag[{0}] invalid 'Default' value '{1}', expected: 'true','false','null'",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    //-- EnumFlag

    public static readonly DiagnosticDescriptor InvalidXml_EnumFlag_InvalidAttribute
        = new("HS107", k_CompilerOptionsTitle,
            "EnumFlag[{0}] invalid attribute '{1}'",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    public static readonly DiagnosticDescriptor InvalidXml_EnumFlag_AttributeName
        = new("HS108", k_CompilerOptionsTitle,
            "EnumFlag required attribute 'Name' was not found",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    public static readonly DiagnosticDescriptor InvalidXml_EnumFlag_AttributeDefault
        = new("HS109", k_CompilerOptionsTitle,
            "EnumFlag[{0}] required attribute 'Default' was not found",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    public static readonly DiagnosticDescriptor InvalidXml_EnumFlag_InvalidAttributeDefault
        = new("HS110", k_CompilerOptionsTitle,
            "EnumFlag[{0}] default value '{1}' was not 'null' and not found amongst 'EnumValue' nodes",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    public static readonly DiagnosticDescriptor InvalidXml_EnumFlag_InvalidNode
       = new("HS111", k_CompilerOptionsTitle,
           "EnumFlag[{0}] expected 'EnumValue' node, got '{0}'",
           k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    //-- EnumValue

    public static readonly DiagnosticDescriptor InvalidXml_EnumValue_InvalidAttribute
        = new("HS112", k_CompilerOptionsTitle,
            "EnumValue[{0}] invalid attribute '{1}'",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    public static readonly DiagnosticDescriptor InvalidXml_EnumValue_AttributeName
        = new("HS113", k_CompilerOptionsTitle,
            "EnumValue required attribute 'Name' was not found",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    //-- StringFlag

    public static readonly DiagnosticDescriptor InvalidXml_StringFlag_InvalidAttribute
        = new("HS114", k_CompilerOptionsTitle,
            "StringFlag[{0}] invalid attribute '{1}'",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);

    public static readonly DiagnosticDescriptor InvalidXml_StringFlag_AttributeName
        = new("HS115", k_CompilerOptionsTitle,
            "StringFlag required attribute 'Name' was not found",
            k_CompilerOptionsCategory, DiagnosticSeverity.Error, true);
}
