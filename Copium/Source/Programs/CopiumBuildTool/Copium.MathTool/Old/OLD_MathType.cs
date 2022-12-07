using System;
using System.Collections.Generic;

namespace Copium.MathTool.Old;


// NOTE(v.matushkin): Some of the ideas on how to improve MathType classes:
//  - Instead of FieldType/NumFields/FieldParam fields have VectorType field for MatrixType,
//    and something like ScalarType for VectorType. So it will be easier to access vector fields from MatrixType.
//  - Add Transpose method for VectorType (and may be also for MatrixType) which will create new VectorType,
//    that just changes between row <=> column vector. This should make things easier in MatrixMulGenerator.


internal enum BaseType
{
    @bool,
    int8,
    int16,
    int32,
    int64,
    uint8,
    uint16,
    uint32,
    uint64,
    float32,
    float64,
}


internal static class OLD_BaseTypeMethods
{
    public static bool IsFloatType(this BaseType baseType) => baseType is BaseType.float32 or BaseType.float64;

    public static int ByteSize(this BaseType baseType)
    {
        return baseType switch
        {
            BaseType.@bool   => 1,
            BaseType.int32   => 4,
            BaseType.uint32  => 4,
            BaseType.float32 => 4,
            BaseType.float64 => 8,
            _ => throw new NotSupportedException(),
        };
    }

    public static string ToMathType(this BaseType baseType, int rows, int columns)
    {
        if (rows < 1 || columns < 1 || (rows == 1 && columns == 1))
        {
            throw new ArgumentException();
        }

        string mathType = baseType switch
        {
            BaseType.@bool   => "Bool",
            BaseType.int32   => "Int",
            BaseType.uint32  => "UInt",
            BaseType.float32 => "Float",
            BaseType.float64 => "Double",
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

    public static string ToValueLiteral(this BaseType baseType, int value)
    {
        string postfix = baseType switch
        {
            BaseType.float32 => ".0f",
            BaseType.float64 => ".0",
            BaseType.int8 or BaseType.int16 or BaseType.int32 or BaseType.int64 => string.Empty,
            BaseType.uint8 or BaseType.uint16 or BaseType.uint32 or BaseType.uint64 => "u",
            _ => throw new NotSupportedException(),
        };

        return value.ToString() + postfix;
    }
}


internal abstract class OLD_MathType
{
    public readonly string TypeDesc;
    public readonly string Type;
    public readonly string TypeParam;
    public readonly bool PassByRef;

    public readonly BaseType BaseTypeEnum;
    public readonly string BaseTypeStr;

    public readonly string FieldType;
    public readonly int NumFields;
    public readonly string FieldParam;

    public readonly int Rows;
    public readonly int Columns;

    public bool IsVector => Rows == 1;
    public bool IsMatrix => IsVector == false;

    public abstract ReadOnlySpan<string> Fields { get; }
    public abstract ReadOnlySpan<string> FieldsLower { get; }


    protected OLD_MathType(string typeDesc, string type, bool typePassByRef, BaseType baseType, string fieldType, int numFields, bool fieldPassByRef, int rows, int columns)
    {
        TypeDesc = typeDesc;
        Type = type;
        TypeParam = _MakeParamType(type, typePassByRef);
        PassByRef = typePassByRef;

        BaseTypeEnum = baseType;
        BaseTypeStr = baseType.ToString();

        FieldType = fieldType;
        NumFields = numFields;
        FieldParam = _MakeParamType(fieldType, fieldPassByRef);

        Rows = rows;
        Columns = columns;
    }


    public int GetRowsOrOneIfVector() => IsVector ? 1 : Rows;
    public int GetColumnsOrOneIfVector() => IsVector ? 1 : Columns;
    public int GetRowsOrDimensionIfVector() => IsVector ? Columns : Rows;


    private static string _MakeParamType(string type, bool passByRef) => passByRef ? $"const {type}&" : type;
}


internal sealed class OLD_VectorType : OLD_MathType
{
    public override ReadOnlySpan<string> Fields => MathToolSettings.VectorFields.AsSpan(0, NumFields);
    public override ReadOnlySpan<string> FieldsLower => MathToolSettings.VectorFieldsLower.AsSpan(0, NumFields);


    private static readonly Dictionary<string, OLD_VectorType> s_Cache = new();


    private OLD_VectorType(string type, bool typePassByRef, BaseType baseType, int dimension)
        : base("Vector", type, typePassByRef, baseType, baseType.ToString(), dimension, false, 1, dimension)
    {
    }


    public static OLD_VectorType Create(BaseType baseType, int dimension)
    {
        OLD_VectorType? vectorType;
        string mathType = baseType.ToMathType(1, dimension);

        if (s_Cache.TryGetValue(mathType, out vectorType))
        {
            return vectorType;
        }
        else
        {
            bool passByRef = baseType.ByteSize() * dimension > 8;
            vectorType = new OLD_VectorType(mathType, passByRef, baseType, dimension);

            s_Cache.Add(mathType, vectorType);
        }

        return vectorType;
    }
}


internal sealed class OLD_MatrixType : OLD_MathType
{
    public override ReadOnlySpan<string> Fields => MathToolSettings.MatrixFields.AsSpan(0, NumFields);
    public override ReadOnlySpan<string> FieldsLower => MathToolSettings.MatrixFieldsLower.AsSpan(0, NumFields);


    private static readonly Dictionary<string, OLD_MatrixType> s_Cache = new();


    private OLD_MatrixType(string type, bool typePassByRef, BaseType baseType, string columnType, int rows, int columns, bool columnPassByRef)
        : base("Matrix", type, typePassByRef, baseType, columnType, columns, columnPassByRef, rows, columns)
    {
    }


    public static OLD_MatrixType Create(BaseType baseType, int rows, int columns)
    {
        OLD_MatrixType? matrixType;
        string mathType = baseType.ToMathType(rows, columns);

        if (s_Cache.TryGetValue(mathType, out matrixType))
        {
            return matrixType;
        }
        else
        {
            bool passByRef = baseType.ByteSize() * rows * columns > 8;
            OLD_VectorType matrixFieldType = OLD_VectorType.Create(baseType, rows);

            matrixType = new OLD_MatrixType(mathType, passByRef, baseType, matrixFieldType.Type, rows, columns, matrixFieldType.PassByRef);

            s_Cache.Add(mathType, matrixType);
        }

        return matrixType;
    }
}
