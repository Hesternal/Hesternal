using System;
using System.IO;

using Hesternal.CSharp;

namespace Hesternal.BuildTool.SourceGenerator;


internal sealed class CSharpWriter : CodeWriter
{
    public static class Keyword
    {
        public const string Bool      = "bool";
        public const string Class     = "class";
        public const string Enum      = "enum";
        public const string Namespace = "namespace";
        public const string Partial   = "partial";
        public const string Sealed    = "sealed";
        public const string Static    = "static";
        public const string Struct    = "struct";
        public const string Using     = "using";
    }

    public static class Visibility
    {
        public const string Internal = "internal";
        public const string Private  = "private";
        public const string Public   = "public";
    }

    [Flags]
    public enum ClassType
    {
        Partial = 1 << 0,
        Sealed  = 1 << 1,
        Static  = 1 << 2,
    }


    public CSharpWriter(Stream stream)
        : base(stream)
    {
    }


    public void WriteUsing(string @using)
    {
        m_codeWriter.Write(Keyword.Using + " "); // Bypass indentation
        m_codeWriter.Write(@using);
        WriteLine(';');
    }

    public void WriteUsingStatic(string @using)
    {
        m_codeWriter.Write(Keyword.Using + " " + Keyword.Static + " "); // Bypass indentation
        m_codeWriter.Write(@using);
        WriteLine(';');
    }

    public void WriteStartUsingStatic() => m_codeWriter.Write(Keyword.Using + " " + Keyword.Static + " "); // Bypass indentation
    public void WriteEndUsingStatic() => WriteLine(';');

    public void WriteNamespace(string @namespace)
    {
        m_codeWriter.Write(Keyword.Namespace + " "); // Bypass indentation
        m_codeWriter.Write(@namespace);
        WriteLine(';');
    }


    public void WriteStartBlock()
    {
        WriteLine('{');
        PushIndent();
    }

    public void WriteEndBlock()
    {
        PopIndent();
        WriteLine('}');
    }

    public void WriteStartStruct(string @struct, string visibility)
    {
        Write(visibility);
        m_codeWriter.Write(" " + Keyword.Struct + " ");
        WriteLine(@struct);
        WriteStartBlock();
    }

    public void WriteEndStruct() => WriteEndBlock();

    public void WriteStartClass(string @class, string visibility, ClassType type)
    {
        Write(visibility);

        if ((type & ClassType.Static) != 0)
        {
            m_codeWriter.Write(" " + Keyword.Static);
        }
        else if ((type & ClassType.Sealed) != 0)
        {
            m_codeWriter.Write(" " + Keyword.Sealed);
        }

        if ((type & ClassType.Partial) != 0)
        {
            m_codeWriter.Write(" " + Keyword.Partial);
        }

        m_codeWriter.Write(" " + Keyword.Class + " ");
        WriteLine(@class);
        WriteStartBlock();
    }

    public void WriteEndClass() => WriteEndBlock();

    public void WriteStartEnum(string @enum, string visibility)
    {
        Write(visibility);
        m_codeWriter.Write(" " + Keyword.Enum + " ");
        WriteLine(@enum);
        WriteStartBlock();
    }

    public void WriteEndEnum() => WriteEndBlock();
}
