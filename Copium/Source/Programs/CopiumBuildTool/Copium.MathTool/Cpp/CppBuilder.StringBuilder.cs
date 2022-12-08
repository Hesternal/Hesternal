using System.Diagnostics.CodeAnalysis;
using System.Runtime.CompilerServices;
using System.Text;
using System;

namespace Copium.MathTool.Cpp;

/*
internal sealed partial class CppBuilder
{
    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(char value, int repeatCount)
    {
        _AppendIndent();
        m_code.Append(value, repeatCount);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(char[]? value, int startIndex, int charCount)
    {
        _AppendIndent();
        m_code.Append(value, startIndex, charCount);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(string? value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(string? value, int startIndex, int count)
    {
        _AppendIndent();
        m_code.Append(value, startIndex, count);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(StringBuilder? value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(StringBuilder? value, int startIndex, int count)
    {
        _AppendIndent();
        m_code.Append(value, startIndex, count);
        return this;
    }


    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder AppendLine()
    {
        _AppendIndent();
        m_shouldIndent = true;
        m_code.AppendLine();
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder AppendLine(string? value)
    {
        _AppendIndent();
        m_shouldIndent = true;
        m_code.AppendLine(value);
        return this;
    }


    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(bool value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(char value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(sbyte value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(byte value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(short value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(int value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(long value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(float value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(double value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(decimal value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(ushort value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(uint value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(ulong value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(object? value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(char[]? value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(ReadOnlySpan<char> value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder Append(ReadOnlyMemory<char> value)
    {
        _AppendIndent();
        m_code.Append(value);
        return this;
    }


    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder AppendFormat([StringSyntax(StringSyntaxAttribute.CompositeFormat)] string format, object? arg0)
    {
        _AppendIndent();
        m_code.AppendFormat(format, arg0);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder AppendFormat([StringSyntax(StringSyntaxAttribute.CompositeFormat)] string format, object? arg0, object? arg1)
    {
        _AppendIndent();
        m_code.AppendFormat(format, arg0, arg1);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder AppendFormat([StringSyntax(StringSyntaxAttribute.CompositeFormat)] string format, object? arg0, object? arg1, object? arg2)
    {
        _AppendIndent();
        m_code.AppendFormat(format, arg0, arg1, arg2);
        return this;
    }

    [MethodImpl(MethodImplOptions.AggressiveInlining)]
    public CppBuilder AppendFormat([StringSyntax(StringSyntaxAttribute.CompositeFormat)] string format, params object?[] args)
    {
        _AppendIndent();
        m_code.AppendFormat(format, args);
        return this;
    }
}
*/
