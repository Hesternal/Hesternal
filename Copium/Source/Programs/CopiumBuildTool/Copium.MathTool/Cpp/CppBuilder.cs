using System;
using System.Text;

namespace Copium.MathTool.Cpp;


internal sealed partial class CppBuilder
{
    private const int k_IndentPerLevel = 4;
    private const char k_IndentChar = ' ';

    private readonly StringBuilder m_code;

    private bool m_shouldIndent;
    private int m_indentSpaces;


    public CppBuilder(int capacity)
    {
        m_code = new(capacity);
    }


    public void BeginNamespace(string name)
    {
        Append("namespace ").AppendLine(name)
            .AppendLine("{");
        Indent();
    }

    public void EndNamespace(string name)
    {
        Unindent();
        Append("} // namespace ").AppendLine(name);
    }


    public void BeginTypeDeclaration(CppStructType cppType)
    {
        Append("struct ").Append(cppType.Name).AppendLine(" final")
            .AppendLine("{");
        Indent();
    }

    public void EndTypeDeclaration()
    {
        Unindent();
        AppendLine("};");
    }


    public void BeginFunction(CppFunction cppFunction)
    {
        if (cppFunction.NoDiscard) Append("[[nodiscard]] ");
        if (cppFunction.Friend) Append("friend ");
        if (cppFunction.Static) Append("static ");

        if (cppFunction.ReturnType is not null) GenerateTypeWithModifiers(cppFunction.ReturnType);
        else Append("void ");

        Append(cppFunction.Name).Append('(');

        for (int i = 0; i < cppFunction.Arguments.Count; i++)
        {
            if (i != 0) Append(", ");

            CppFunctionArgumentType cppFunctionArgument = cppFunction.Arguments[i];
            GenerateTypeWithModifiers(cppFunctionArgument);
            Append(cppFunctionArgument.Name);
        }

        AppendLine("{");
        Indent();

        void GenerateTypeWithModifiers(CppTypeWithModifiers type)
        {
            if (type.Const) Append("const ");
            Append(type.Type.Name);
            if (type.Ref) Append('&');
            Append(' ');
        }
    }

    public void EndFunction()
    {
        Unindent();
        AppendLine("}");
    }


    public void Indent()
    {
        m_indentSpaces += k_IndentPerLevel;
    }

    public void Unindent()
    {
        m_indentSpaces -= k_IndentPerLevel;
        if (m_indentSpaces < 0)
        {
            throw new InvalidOperationException();
        }
    }

    private void _AppendIndent()
    {
        if (m_shouldIndent)
        {
            m_shouldIndent = false;
            m_code.Append(k_IndentChar, m_indentSpaces);
        }
    }
}
