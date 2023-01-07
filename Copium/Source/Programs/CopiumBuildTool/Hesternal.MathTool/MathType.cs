using System;
using System.Collections.Generic;

using Copium.MathTool.Cpp;

namespace Copium.MathTool;


internal static class CppSystemTypeMethods
{
    public static bool IsFloatType(this CppSystemType systemType) => systemType is CppSystemType.float32 or CppSystemType.float64;

    public static int ByteSize(this CppSystemType systemType)
    {
        return systemType switch
        {
            CppSystemType.@bool => 1,
            CppSystemType.int32 => 4,
            CppSystemType.uint32 => 4,
            CppSystemType.float32 => 4,
            CppSystemType.float64 => 8,
            _ => throw new NotSupportedException(),
        };
    }

    public static string ToMathType(this CppSystemType systemType, int rows, int columns)
    {
        if (rows < 1 || columns < 1 || (rows == 1 && columns == 1))
        {
            throw new ArgumentException();
        }

        string mathType = systemType switch
        {
            CppSystemType.@bool => "Bool",
            CppSystemType.int32 => "Int",
            CppSystemType.uint32 => "UInt",
            CppSystemType.float32 => "Float",
            CppSystemType.float64 => "Double",
            _ => throw new NotSupportedException(),
        };

        if (rows == 1 || columns == 1)
        {
            mathType += MathF.Max(columns, rows);
        }
        else
        {
            mathType += $"{rows}x{columns}";
        }

        return mathType;
    }

    public static string ToValueLiteral(this CppSystemType systemType, int value)
    {
        string postfix = systemType switch
        {
            CppSystemType.float32 => ".0f",
            CppSystemType.float64 => ".0",
            CppSystemType.int8 or CppSystemType.int16 or CppSystemType.int32 or CppSystemType.int64 => string.Empty,
            CppSystemType.uint8 or CppSystemType.uint16 or CppSystemType.uint32 or CppSystemType.uint64 => "u",
            _ => throw new NotSupportedException(),
        };

        return value.ToString() + postfix;
    }
}


internal abstract class MathType : CppStructType
{
    public readonly int Rows;
    public readonly int Columns;

    public readonly CppType BaseSystemType;
    public readonly string TypeDesc;

    public readonly CppType FieldType;
    public readonly int NumFields;
    public abstract ReadOnlySpan<string> FieldNames { get; }


    protected static readonly Dictionary<string, MathType> s_TypeCache = new();


    protected MathType(string name, CppSystemType systemType, int rows, int columns, string typeDesc, CppType fieldType, int numFields)
        : base(name, systemType.ByteSize() * rows * columns > 8)
    {
        Rows = rows;
        Columns = columns;

        BaseSystemType = new CppFundamentalType(systemType);
        TypeDesc = typeDesc;

        FieldType = fieldType;
        NumFields = numFields;
    }
}


internal sealed class VectorType : MathType
{
    public override ReadOnlySpan<string> FieldNames => MathToolSettings.VectorFields.AsSpan(0, NumFields);


    private VectorType(string name, CppSystemType systemType, int rows, int columns)
        : base(name, systemType, rows, columns, "Vector", new CppFundamentalType(systemType), Math.Max(rows, columns))
    {
    }


    public static VectorType CreateRowVector(CppSystemType systemType, int dimension) => _Create(systemType, dimension, 1);
    public static VectorType CreateColumnVector(CppSystemType systemType, int dimension) => _Create(systemType, 1, dimension);


    private static VectorType _Create(CppSystemType systemType, int rows, int columns)
    {
        string mathTypeName = systemType.ToMathType(rows, columns);

        if (s_TypeCache.TryGetValue(mathTypeName, out MathType? vectorMathType))
        {
            return (VectorType)vectorMathType;
        }

        VectorType vectorType = new(mathTypeName, systemType, rows, columns);
        s_TypeCache.Add(mathTypeName, vectorType);

        return vectorType;
    }
}


internal sealed class MatrixType : MathType
{
    public override ReadOnlySpan<string> FieldNames => MathToolSettings.MatrixFields.AsSpan(0, NumFields);


    public MatrixType(string name, CppSystemType systemType, int rows, int columns)
        : base(name, systemType, rows, columns, "Matrix", VectorType.CreateColumnVector(systemType, rows), columns)
    {
    }


    public static MatrixType Create(CppSystemType systemType, int rows, int columns)
    {
        string mathTypeName = systemType.ToMathType(rows, columns);

        if (s_TypeCache.TryGetValue(mathTypeName, out MathType? matrixMathType))
        {
            return (MatrixType)matrixMathType;
        }

        MatrixType matrixType = new(mathTypeName, systemType, rows, columns);
        s_TypeCache.Add(mathTypeName, matrixType);

        return matrixType;
    }
}