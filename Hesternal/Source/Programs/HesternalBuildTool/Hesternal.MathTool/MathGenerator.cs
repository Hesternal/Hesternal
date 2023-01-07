using System.Collections.Generic;
using System.IO;

using Hesternal.MathTool.Cpp;

namespace Hesternal.MathTool;

/*
internal abstract class MathGenerator
{
//    protected static class Indent
//    {
//        private const string k_OneLevel = "    ";
//
//        public const string Namespace = k_OneLevel;
//
//        public const string Class  = k_OneLevel + Namespace;
//        public const string Method = k_OneLevel + Class;
//
//        public const string FreeFunction = k_OneLevel + Namespace;
//    }

    protected readonly CppBuilder m_cb;


    private string? m_namespace;


    protected MathGenerator()
    {
        m_cb = new CppBuilder(MathToolSettings.SbInitialCapacity);
    }


    protected void Start(string moduleName, IEnumerable<string>? moduleImports, IEnumerable<string>? headerImports, string @namespace)
    {
        m_namespace = @namespace;

        m_cb.AppendLine(MathToolSettings.AutoGenComment)
            .AppendFormat("export module {0};", moduleName).AppendLine()
            .AppendLine();

        if (moduleImports is not null)
        {
            foreach (string moduleImport in moduleImports)
            {
                m_cb.AppendFormat("import {0};", moduleImport).AppendLine();
            }
        }

        if (headerImports is not null)
        {
            m_cb.AppendLine();

            foreach (string headerImport in headerImports)
            {
                m_cb.AppendFormat("import {0};", headerImport).AppendLine();
            }
        }

        m_cb.AppendLine()
            .AppendLine()
            .Append("export namespace ").AppendLine(m_namespace)
            .AppendLine("{")
            .AppendLine();
    }

    protected void End(string filePath)
    {
        m_cb.AppendLine()
            .Append("} // export namespace ").AppendLine(m_namespace!);

        File.WriteAllText(filePath, m_cb.ToString());
    }
}
*/
