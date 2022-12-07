using System.Text;

namespace Copium.MathTool.Cpp;


internal partial class CppGenerator
{
    private const int k_IndentPerLevel = 4;
    private const char k_IndentChar = ' ';


    private int m_indentLevel;
    private int m_indentSpaces;

    private readonly StringBuilder m_sb = new();


    public CppGenerator()
    {
    }


    // public CppNamespaceBuilder CreateNamespace(string name) => new(this, name);


    private void _Indent()
    {
        m_indentLevel++;
        m_indentSpaces += k_IndentPerLevel;
    }

    private void _Unindent()
    {
        m_indentLevel--;
        m_indentSpaces -= k_IndentPerLevel;
    }

    private void _AppendIndent()
    {
        m_sb.Append(k_IndentChar, m_indentSpaces);
    }
}
