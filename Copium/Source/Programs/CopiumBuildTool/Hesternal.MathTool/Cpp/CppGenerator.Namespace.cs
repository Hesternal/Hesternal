using System;

namespace Copium.MathTool.Cpp;


internal partial class CppGenerator
{
    public void BeginNamespace(string name)
    {
        _AppendIndent();
        m_sb.Append("namespace ").AppendLine(name);
        _Indent();
    }

    public void EndNamespace(string name)
    {
        _Unindent();
        _AppendIndent();
        m_sb.Append("} // namespace ").AppendLine(name);
    }


//    public sealed class CppNamespaceBuilder : IDisposable
//    {
//        private readonly CppGenerator m_cppGenerator;
//        private readonly string m_name;
//
//
//        public CppNamespaceBuilder(CppGenerator cppGenerator, string name)
//        {
//            m_cppGenerator = cppGenerator;
//            m_name = name;
//
//            m_cppGenerator._Indent();
//            m_cppGenerator._AppendIndent();
//            m_cppGenerator.m_sb.Append("namespace ").AppendLine(name);
//            m_cppGenerator._AppendIndent();
//            m_cppGenerator.m_sb.AppendLine("{");
//        }
//
//        public void Dispose()
//        {
//            m_cppGenerator._Unindent();
//            m_cppGenerator._AppendIndent();
//            m_cppGenerator.m_sb.Append("} // namespace ").AppendLine(m_name);
//        }
//    }
}
