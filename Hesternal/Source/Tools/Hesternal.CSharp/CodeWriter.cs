using System;
using System.IO;

namespace Hesternal.CSharp;


public abstract class CodeWriter : IDisposable
{
    private const int k_IndentSize = 4;
    // NOTE(v.matushkin): No ReadOnlySpan<char> in .Net Standard so use char[] instead of string
    private static readonly char[] s_IndentString = "                                                ".ToCharArray();

    private bool m_shouldIndent;
    private int m_indentLevel;
    private int m_indentSpaces;

    protected readonly StreamWriter m_codeWriter;


    protected CodeWriter(Stream stream)
    {
        m_codeWriter = new StreamWriter(stream);
    }

    public void Flush() => m_codeWriter.Flush();
    public void Dispose() => m_codeWriter.Dispose();


    public void Write(int value)
    {
        _WriteIndent();
        m_codeWriter.Write(value);
    }

    public void Write(char value)
    {
        _WriteIndent();
        m_codeWriter.Write(value);
    }

    public void Write(string value)
    {
        _WriteIndent();
        m_codeWriter.Write(value);
    }

    public void Write(string format, object arg0)
    {
        _WriteIndent();
        m_codeWriter.Write(format, arg0);
    }

    public void Write(string format, object arg0, object arg1)
    {
        _WriteIndent();
        m_codeWriter.Write(format, arg0, arg1);
    }

    public void Write(string format, object arg0, object arg1, object arg2)
    {
        _WriteIndent();
        m_codeWriter.Write(format, arg0, arg1, arg2);
    }

    public void Write(string format, params object[] arg)
    {
        _WriteIndent();
        m_codeWriter.Write(format, arg);
    }

    public void WriteLine()
    {
        m_codeWriter.WriteLine();
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

    public void WriteLine(string format, object arg0)
    {
        Write(format, arg0);
        WriteLine();
    }

    public void WriteLine(string format, object arg0, object arg1)
    {
        Write(format, arg0, arg1);
        WriteLine();
    }

    public void WriteLine(string format, object arg0, object arg1, object arg2)
    {
        Write(format, arg0, arg1, arg2);
        WriteLine();
    }

    public void WriteLine(string format, params object[] arg)
    {
        Write(format, arg);
        WriteLine();
    }

    public void WriteLines(int lines)
    {
        for (int i = 0; i < lines; i++)
        {
            m_codeWriter.WriteLine();
        }
        m_shouldIndent = true;
    }


    protected void PushIndent() => m_indentSpaces = ++m_indentLevel * k_IndentSize;
    protected void PopIndent()  => m_indentSpaces = --m_indentLevel * k_IndentSize;


    private void _WriteIndent()
    {
        if (m_shouldIndent)
        {
            m_shouldIndent = false;
            m_codeWriter.Write(s_IndentString, 0, m_indentSpaces);
        }
    }
}
