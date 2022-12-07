using System;
using System.Collections.Generic;

namespace Copium.MathTool.Cpp;


enum CppTypeKind
{
    Fudamental,
    Struct,
}

// NOTE(v.matushkin): Naming?
enum CppSystemType
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


internal abstract class CppSymbol
{
    public readonly string Name;


    protected CppSymbol(string name)
    {
        Name = name;
    }
}


internal abstract class CppType : CppSymbol
{
    public readonly CppTypeKind TypeKind;

    public readonly bool PassByRef;
    public readonly CppTypeWithModifiers TypeParam;


    protected CppType(string name, CppTypeKind typeKind, bool passByRef)
        : base(name)
    {
        TypeKind = typeKind;

        PassByRef = passByRef;
        TypeParam = new()
        {
            Type = this,
            Const = passByRef,
            Ref = passByRef,
        };
    }
}


internal sealed class CppFundamentalType : CppType
{
    public CppFundamentalType(CppSystemType systemType)
        : base(systemType.ToString(), CppTypeKind.Fudamental, false)
    {
    }
}


internal class CppTypeWithModifiers
{
    public required CppType Type { get; init; }
    public bool Const { get; init; }
    public bool Ref { get; init; }
}

internal sealed class CppFunctionArgument
{
    public required CppTypeWithModifiers Type { get; init; }
    public required string Name { get; init; }
}


internal class CppFunction : CppSymbol
{
    public readonly CppTypeWithModifiers? ReturnType;
    public readonly List<CppFunctionArgument> Arguments = new();

    public bool NoDiscard { get; init; }
    public bool Friend { get; init; }
    public bool Static { get; init; }
    public bool Constexpr { get; init; }
    public bool NoExcept { get; init; }


    public CppFunction(string name, CppTypeWithModifiers? returnType)
        : base(name)
    {
        ReturnType = returnType;
    }
}


internal sealed class CppField
{
    public readonly CppType Type;
    public readonly string Name;


    public CppField(CppType type, string name)
    {
        Type = type;
        Name = name;
    }
}


internal class CppStructType : CppType
{
    //public readonly List<CppField> Fields = new();
    public readonly List<CppFunction> Constructors = new();
    public readonly List<CppFunction> Methods = new();


    public CppStructType(string name, bool passByRef)
        : base(name, CppTypeKind.Struct, passByRef)
    {
    }
}
