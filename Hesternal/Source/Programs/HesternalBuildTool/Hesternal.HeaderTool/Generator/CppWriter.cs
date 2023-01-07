using System;
using System.IO;

namespace Hesternal.HeaderTool.Generator
{
    internal sealed class CppWriter : IDisposable
    {
        private const int k_IndentSize = 4;
        // NOTE(v.matushkin): No ReadOnlySpan<char> in .Net Standard so use char[] instead of string
        private static readonly char[] s_IndentString = "                                                ".ToCharArray();

        private readonly StreamWriter m_cppFile;
        private bool m_disposed = false;

        private bool m_shouldIndent;
        private int m_indentLevel;
        private int m_indentSpaces;


        public CppWriter(string cppPath)
        {
            m_cppFile = new StreamWriter(cppPath, false);
        }

        public CppWriter(FileInfo file)
        {
            m_cppFile = file.CreateText();
        }

        ~CppWriter()
        {
            _Dispose(disposing: false);
        }

        public void Dispose()
        {
            _Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        private void _Dispose(bool disposing)
        {
            if (m_disposed == false)
            {
                m_disposed = true;
                if (disposing)
                {
                    m_cppFile.Dispose();
                }
            }
        }


        public void Write(int value)
        {
            _WriteIndent();
            m_cppFile.Write(value);
        }

        public void Write(char value)
        {
            _WriteIndent();
            m_cppFile.Write(value);
        }

        public void Write(string value)
        {
            _WriteIndent();
            m_cppFile.Write(value);
        }

        public void WriteLine()
        {
            m_cppFile.WriteLine();
            m_shouldIndent = true;
        }

        public void WriteLine(char value)
        {
            Write(value);
            WriteLine();
        }

        public void WriteLine(string value)
        {
            Write(value);
            WriteLine();
        }


        public void WriteFileHeader_Hpp(string[] includes, string[] moduleImports)
        {
            WriteLine("#pragma once");
            WriteLine();

            if (includes != null && includes.Length != 0)
            {
                _WriteIncludes(includes, bExternal: false);
                WriteLine();
            }

            if (moduleImports != null && moduleImports.Length != 0)
            {
                _WriteImports(moduleImports);
                WriteLine();
            }

            WriteLine();
        }

        public void WriteFileHeader_ModuleImplementation(string moduleName, string[] includes, string[] stdIncludes, string[] moduleImports)
        {
            bool hasIncludes = includes != null && includes.Length != 0;
            bool hasStdIncludes = stdIncludes != null && stdIncludes.Length != 0;

            if (hasIncludes || hasStdIncludes)
            {
                WriteLine("module;");
                WriteLine();

                if (hasIncludes)
                {
                    _WriteIncludes(includes, bExternal: false);
                    WriteLine();
                }

                if (hasStdIncludes)
                {
                    _WriteIncludes(stdIncludes, bExternal: true);
                    WriteLine();
                }
            }

            Write("module ");
            Write(moduleName);
            WriteLine(';');
            WriteLine();

            if (moduleImports != null && moduleImports.Length != 0)
            {
                _WriteImports(moduleImports);
                WriteLine();
            }

            WriteLine();
        }


        public void BeginBlock()
        {
            WriteLine('{');
            _PushIndent();
        }

        public void EndBlock()
        {
            _PopIndent();
            WriteLine('}');
        }

        public void BeginInitializerList()
        {
            BeginBlock();
        }

        public void EndInitializerList()
        {
            _PopIndent();
            WriteLine("};");
        }

        public void BeginNamespace(string namespaceName)
        {
            Write("namespace ");
            WriteLine(namespaceName);

            BeginBlock();
        }

        public void EndNamespace()
        {
            EndBlock();
        }


        private void _WriteIndent()
        {
            if (m_shouldIndent)
            {
                m_shouldIndent = false;
                m_cppFile.Write(s_IndentString, 0, m_indentSpaces);
            }
        }

        private void _WriteImports(string[] imports)
        {
            foreach (string import in imports)
            {
                Write("import ");
                Write(import);
                WriteLine(';');
            }
        }

        private void _WriteIncludes(string[] includes, bool bExternal)
        {
            char includeOpen;
            char includeClose;

            if (bExternal)
            {
                includeOpen = '<';
                includeClose = '>';
            }
            else
            {
                includeOpen = '"';
                includeClose = '"';
            }

            foreach (string include in includes)
            {
                Write("#include ");
                Write(includeOpen);
                Write(include);
                WriteLine(includeClose);
            }
        }


        private void _PushIndent()
        {
            m_indentSpaces = ++m_indentLevel * k_IndentSize;
        }

        private void _PopIndent()
        {
            m_indentSpaces = --m_indentLevel * k_IndentSize;
        }
    }
}
