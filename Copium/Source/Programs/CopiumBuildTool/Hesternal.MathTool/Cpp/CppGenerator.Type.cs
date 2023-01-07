using System;

namespace Hesternal.MathTool.Cpp;


internal partial class CppGenerator
{
    public void BeginTypeDeclaration(CppStructType cppType)
    {
        _AppendIndent();
        m_sb.Append("struct ").Append(cppType.Name).AppendLine(" final");
        _AppendIndent();
        m_sb.AppendLine("{");
        _Indent();
    }

    public void EndTypeDeclaration()
    {
        _Unindent();
        _AppendIndent();
        m_sb.AppendLine("};");
    }


//    public sealed class CppTypeBuilder : IDisposable
//    {
//        private readonly CppGenerator m_cppGenerator;
//        private readonly CppStructType m_cppType;
//
//
//        public CppTypeBuilder(CppGenerator cppGenerator, CppStructType cppType)
//        {
//            m_cppGenerator = cppGenerator;
//            m_cppType = cppType;
//
//            m_cppGenerator._Indent();
//            m_cppGenerator._AppendIndent();
//            m_cppGenerator.m_sb.Append("struct ").Append(m_cppType.Name).AppendLine(" final");
//            m_cppGenerator._AppendIndent();
//            m_cppGenerator.m_sb.AppendLine("{");
//        }
//
//        public void Dispose()
//        {
//            m_cppGenerator._Unindent();
//            m_cppGenerator._AppendIndent();
//            m_cppGenerator.m_sb.AppendLine("};");
//        }
//    }
}
