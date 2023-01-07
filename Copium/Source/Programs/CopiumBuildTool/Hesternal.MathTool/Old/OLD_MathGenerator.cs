using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Hesternal.MathTool.Old;


internal abstract class OLD_MathGenerator
{
    protected static class Indent
    {
        private const string k_OneLevel = "    ";

        public const string Namespace = k_OneLevel;

        public const string Class  = k_OneLevel + Namespace;
        public const string Method = k_OneLevel + Class;

        public const string FreeFunction = k_OneLevel + Namespace;
    }

    protected readonly StringBuilder m_sb;


    private string? m_namespace;


    protected OLD_MathGenerator()
    {
        m_sb = new StringBuilder(MathToolSettings.SbInitialCapacity);
    }


    protected void Start(string moduleName, IEnumerable<string>? moduleImports, IEnumerable<string>? headerImports, string @namespace)
    {
        m_namespace = @namespace;

        m_sb.AppendLine(MathToolSettings.AutoGenComment)
            .AppendFormat("export module {0};", moduleName).AppendLine()
            .AppendLine();

        if (moduleImports is not null)
        {
            foreach (string moduleImport in moduleImports)
            {
                m_sb.AppendFormat("import {0};", moduleImport).AppendLine();
            }
        }

        if (headerImports is not null)
        {
            m_sb.AppendLine();

            foreach (string headerImport in headerImports)
            {
                m_sb.AppendFormat("import {0};", headerImport).AppendLine();
            }
        }

        m_sb.AppendLine()
            .AppendLine()
            .Append("export namespace ").AppendLine(m_namespace)
            .AppendLine("{")
            .AppendLine();
    }

    protected void End(string filePath)
    {
        m_sb.AppendLine()
            .Append("} // export namespace ").AppendLine(m_namespace!);

        File.WriteAllText(filePath, m_sb.ToString());
    }
}
