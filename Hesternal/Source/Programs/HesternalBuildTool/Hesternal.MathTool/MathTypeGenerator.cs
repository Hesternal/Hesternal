using System;
using System.Collections.Generic;
using System.Linq;

using Hesternal.MathTool.Cpp;

namespace Hesternal.MathTool;

/*
internal abstract class MathTypeGenerator : MathGenerator
{
    protected static readonly string[] CompoundOperators = { "+=", "-=", "*=" };
    protected static readonly string[] BinaryOperators = { "+", "-", "*" };

    protected string TypeDesc => m_mathType.TypeDesc;
    //protected string Type => m_mathType.Type;
    protected CppTypeWithModifiers TypeParam => m_mathType.TypeParam;

    //protected BaseType BaseTypeEnum => m_mathType.BaseTypeEnum;
    protected CppType BaseSystemType => m_mathType.BaseSystemType;

    protected CppType FieldType => m_mathType.FieldType;
    protected int NumFields => m_mathType.NumFields;
    //protected string FieldParam => m_mathType.FieldParam;
    protected ReadOnlySpan<string> FieldNames => m_mathType.FieldNames;
    //protected ReadOnlySpan<string> FieldsLower => m_mathType.FieldsLower;

    protected int Rows => m_mathType.Rows;
    protected int Columns => m_mathType.Columns;

    protected readonly CppTypeWithModifiers ReturnType;
    protected readonly CppTypeWithModifiers ReturnRefType;

    protected readonly CppFunctionArgument LhsScalar;
    protected readonly CppFunctionArgument RhsScalar;
    protected readonly CppFunctionArgument LhsMathType;
    protected readonly CppFunctionArgument RhsMathType;


    private readonly MathType m_mathType;


    protected MathTypeGenerator(MathType mathType)
    {
        m_mathType = mathType;

        ReturnType = new() { Type = mathType };
        ReturnRefType = new() { Type = mathType, Ref = true };

        LhsScalar = new() { Type = mathType.BaseSystemType.TypeParam, Name = "lhs" };
        RhsScalar = new() { Type = mathType.BaseSystemType.TypeParam, Name = "rhs" };
        LhsMathType = new() { Type = mathType.TypeParam, Name = "lhs" };
        RhsMathType = new() { Type = mathType.TypeParam, Name = "rhs" };
    }


    protected void Generate(string filePath, string moduleName)
    {
        Start(moduleName, ModuleImports.Append("HesternalEngine.Core.CoreTypes"), null, MathToolSettings.Namespace);

        m_cb.BeginTypeDeclaration(m_mathType);
        //m_cb.AppendFormat("struct {0} final", Type).AppendLine()
        //    .AppendLine("{");

        _GenerateFields();
        m_cb.AppendLine()
            .AppendLine();

        _GenerateCommonConstructors();
        GenerateConstructors();
        m_cb.AppendLine();

        _GenerateCompoundOperators();
        m_cb.AppendLine();

        _GenerateUnaryOperators();
        m_cb.AppendLine();

        _GenerateBinaryOperators();
        m_cb.AppendLine();

        GenerateStaticMethods();

        m_cb.EndTypeDeclaration();
        //m_cb.AppendLine("};");

        End(filePath);
    }


    protected abstract IEnumerable<string> ModuleImports { get; }

    protected virtual void GenerateConstructors()
    {
    }

    protected abstract void GenerateStaticMethods();


    private void _GenerateFields()
    {
        foreach (string fieldName in FieldNames)
        {
            m_cb.AppendFormat("{0} {1};", FieldType.Name, fieldName).AppendLine();
        }
    }

    private void _GenerateCommonConstructors()
    {
        //- Default empty constructor
        m_cb.AppendFormat("constexpr {0}() = default;", Type).AppendLine()
        //- Default copy constructor
            .AppendFormat("constexpr {0}(const {0}& other) = default;", Type).AppendLine()
            .AppendLine();

        int numFields = NumFields;
        ReadOnlySpan<string> fields = Fields;

        //- Scalar constructor
        m_cb.AppendFormat("explicit constexpr {0}({1} scalar)", Type, BaseType).AppendLine();
        for (int i = 0; i < numFields; i++)
        {
            char c = i == 0 ? ':' : ',';
            m_cb.AppendFormat("{0} {1}(scalar)", c, fields[i]).AppendLine();
        }
        m_cb.AppendLine("{}");

        ReadOnlySpan<string> fieldsLower = FieldsLower;

        //- Field constructor
        m_cb.AppendFormat("constexpr {0}(", Type);
        //-- Params
        for (int i = 0; i < numFields; i++)
        {
            if (i != 0)
            {
                m_cb.Append(", ");
            }
            m_cb.AppendFormat("{0} {1}", FieldParam, fieldsLower[i]);
        }
        m_cb.AppendLine(")");
        //-- Initializer list
        for (int i = 0; i < numFields; i++)
        {
            char c = i == 0 ? ':' : ',';
            m_cb.AppendFormat("{0} {1}({2})", c, fields[i], fieldsLower[i]).AppendLine();
        }
        m_cb.AppendLine("{}");
    }

    private void _GenerateCompoundOperators()
    {
        m_cb.AppendLine("//- Compound");

        m_cb.AppendLine("//-- Vector | Scalar");
        WriteOperators(RhsScalar, true);

        m_cb.AppendLine("//-- Vector | Vector");
        WriteOperators(RhsMathType, false);

        void WriteOperators(CppFunctionArgument rhsArgument, bool rhsIsScalar)
        {
            foreach (string op in CompoundOperators)
            {
                CppFunction compoundOperator = new("operator" + op, ReturnRefType)
                {
                    Friend = true,
                    Constexpr = true,
                };
                compoundOperator.Arguments.Add(rhsArgument);

                m_mathType.Methods.Add(compoundOperator);

                m_cb.BeginFunction(compoundOperator);

                foreach (string fieldName in FieldNames)
                {
                    m_cb.AppendFormat("{0} {1} rhs", fieldName, op);
                    if (rhsIsScalar == false)
                    {
                        m_cb.Append('.').Append(fieldName);
                    }
                    m_cb.Append("; ");
                }
                m_cb.Append("return *this; ");

                m_cb.EndFunction();
            }
        }
    }

    private void _GenerateUnaryOperators()
    {
        CppFunction minusOperator = new("operator-", ReturnType)
        {
            Friend = true,
            Constexpr = true,
        };
        minusOperator.Arguments.Add(RhsMathType);
        m_mathType.Methods.Add(minusOperator);

        m_cb.AppendLine("//- Unary");
        m_cb.BeginFunction(minusOperator);

        m_cb.AppendFormat("return {0}(", m_mathType.Name);
        ReadOnlySpan<string> fieldNames = FieldNames;
        for (int i = 0; i < NumFields; i++)
        {
            if (i != 0)
            {
                m_cb.Append(", ");
            }
            m_cb.Append("-rhs.").Append(fieldNames[i]);
        }
        m_cb.Append(");");

        m_cb.EndFunction();
    }

    private void _GenerateBinaryOperators()
    {
        m_cb.AppendLine("//- Binary");

        m_cb.AppendFormat("//-- {0} | Scalar", TypeDesc).AppendLine();
        WriteOperators(LhsMathType, RhsScalar, false, true);

        m_cb.AppendFormat("//-- Scalar | {0}", TypeDesc).AppendLine();
        WriteOperators(LhsScalar, RhsMathType, true, false);

        m_cb.AppendFormat("//-- {0} | {0}", TypeDesc).AppendLine();
        WriteOperators(LhsMathType, RhsMathType, false, false);

        void WriteOperators(CppFunctionArgument lhsArgument, CppFunctionArgument rhsArgument, bool lhsIsScalar, bool rhsIsScalar)
        {
            ReadOnlySpan<string> fieldNames = FieldNames;
            foreach (string op in BinaryOperators)
            {
                CppFunction binaryOperator = new("operator" + op, ReturnType)
                {
                    Friend = true,
                    Constexpr = true,
                };
                binaryOperator.Arguments.Add(lhsArgument);
                binaryOperator.Arguments.Add(rhsArgument);

                m_mathType.Methods.Add(binaryOperator);

                m_cb.BeginFunction(binaryOperator);

                m_cb.AppendFormat("return {0}(", m_mathType.Name);
                for (int i = 0; i < NumFields; i++)
                {
                    if (i != 0)
                    {
                        m_cb.Append(", ");
                    }

                    string format;
                    if (lhsIsScalar)
                    {
                        format = "lhs {0} rhs.{1}";
                    }
                    else if (rhsIsScalar)
                    {
                        format = "lhs.{1} {0} rhs";
                    }
                    else
                    {
                        format = "lhs.{1} {0} rhs.{1}";
                    }

                    m_cb.AppendFormat(format, op, fieldNames[i]);
                }
                m_cb.Append("); ");

                m_cb.EndFunction();
            }
        }
    }
}
*/
