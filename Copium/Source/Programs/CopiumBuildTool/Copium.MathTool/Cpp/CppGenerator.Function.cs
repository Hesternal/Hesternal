using System;
using System.Text;

namespace Copium.MathTool.Cpp;


internal partial class CppGenerator
{
    public void GenerateFunction(CppFunction cppFunction, Action<StringBuilder> functionBody)
    {
        if (cppFunction.NoDiscard) m_sb.Append("[[nodiscard]] ");
        if (cppFunction.Friend) m_sb.Append("friend ");
        if (cppFunction.Static) m_sb.Append("static ");

        if (cppFunction.ReturnType is not null) GenerateTypeWithModifiers(m_sb, cppFunction.ReturnType);
        else m_sb.Append("void ");

        m_sb.Append(cppFunction.Name).Append('(');

        for (int i = 0; i < cppFunction.Arguments.Count; i++)
        {
            if (i != 0) m_sb.Append(", ");

            CppFunctionArgumentType cppFunctionArgument = cppFunction.Arguments[i];
            GenerateTypeWithModifiers(m_sb, cppFunctionArgument);
            m_sb.Append(cppFunctionArgument.Name);
        }

        _AppendIndent();
        m_sb.AppendLine("{");
        _Indent();

        functionBody(m_sb);

        _Unindent();
        _AppendIndent();
        m_sb.AppendLine("}");

        static void GenerateTypeWithModifiers(StringBuilder code, CppTypeWithModifiers type)
        {
            if (type.Const) code.Append("const ");
            code.Append(type.Type.Name);
            if (type.Ref) code.Append('&');
            code.Append(' ');
        }
    }


//    public sealed class CppFunctionBuilder : IDisposable
//    {
//        private readonly CppGenerator m_cppGenerator;
//        private readonly CppFunction m_cppFunction;
//
//
//        public CppFunctionBuilder(CppGenerator cppGenerator, CppFunction cppFunction)
//        {
//            m_cppGenerator = cppGenerator;
//            m_cppFunction = cppFunction;
//
//            m_cppGenerator._Indent();
//            m_cppGenerator._AppendIndent();
//
//            StringBuilder code = m_cppGenerator.m_sb;
//
//            if (cppFunction.NoDiscard) code.Append("[[nodiscard]] ");
//            if (cppFunction.Static) code.Append("static ");
//
//            if (cppFunction.ReturnType is not null) code.Append(cppFunction.ReturnType.Name).Append(' ');
//            else code.Append("void ");
//
//            code.Append(cppFunction.Name).Append('(');
//
//            m_cppGenerator._AppendIndent();
//            code.AppendLine("{");
//        }
//
//        public void Dispose()
//        {
//            m_cppGenerator._Unindent();
//            m_cppGenerator._AppendIndent();
//            m_cppGenerator.m_sb.AppendLine("}");
//        }
//    }
}
