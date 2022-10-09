using System;
using System.Collections.Generic;
using System.Linq;

namespace Copium.MathTool;


internal abstract class MathTypeGenerator : MathGenerator
{
    protected static readonly string[] CompoundOperators = { "+=", "-=", "*=" };
    protected static readonly char[] BinaryOperators = { '+', '-', '*' };

    protected string TypeDesc => m_mathType.TypeDesc;
    protected string Type => m_mathType.Type;
    protected string TypeParam => m_mathType.TypeParam;

    protected BaseType BaseTypeEnum => m_mathType.BaseTypeEnum;
    protected string BaseType => m_mathType.BaseTypeStr;

    protected string FieldType => m_mathType.FieldType;
    protected int NumFields => m_mathType.NumFields;
    protected string FieldParam => m_mathType.FieldParam;
    protected ReadOnlySpan<string> Fields => m_mathType.Fields;
    protected ReadOnlySpan<string> FieldsLower => m_mathType.FieldsLower;

    protected int Rows => m_mathType.Rows;
    protected int Columns => m_mathType.Columns;


    private readonly MathType m_mathType;


    protected MathTypeGenerator(MathType mathType)
    {
        m_mathType = mathType;
    }


    protected void Generate(string filePath, string moduleName)
    {
        Start(moduleName, ModuleImports.Append("CopiumEngine.Core.CoreTypes"), null, MathToolSettings.Namespace);

        m_sb.AppendFormat(Indent.Namespace + "struct {0} final", Type).AppendLine()
            .AppendLine(Indent.Namespace + "{");

        _GenerateFields();
        m_sb.AppendLine()
            .AppendLine();

        _GenerateCommonConstructors();
        GenerateConstructors();
        m_sb.AppendLine();

        _GenerateCompoundOperators();
        m_sb.AppendLine();

        _GenerateUnaryOperators();
        m_sb.AppendLine();

        _GenerateBinaryOperators();
        m_sb.AppendLine();

        GenerateStaticMethods();

        m_sb.AppendLine(Indent.Namespace + "};");

        End(filePath);
    }


    protected abstract IEnumerable<string> ModuleImports { get; }

    protected virtual void GenerateConstructors()
    {
    }

    protected abstract void GenerateStaticMethods();


    private void _GenerateFields()
    {
        foreach (string field in Fields)
        {
            m_sb.AppendFormat(Indent.Class + "{0} {1};", FieldType, field).AppendLine();
        }
    }

    private void _GenerateCommonConstructors()
    {
        //- Default empty constructor
        m_sb.AppendFormat(Indent.Class + "constexpr {0}() = default;", Type).AppendLine()
        //- Default copy constructor
            .AppendFormat(Indent.Class + "constexpr {0}(const {0}& other) = default;", Type).AppendLine()
            .AppendLine();

        int numFields = NumFields;
        ReadOnlySpan<string> fields = Fields;

        //- Scalar constructor
        m_sb.AppendFormat(Indent.Class + "explicit constexpr {0}({1} scalar)", Type, BaseType).AppendLine();
        for (int i = 0; i < numFields; i++)
        {
            char c = i == 0 ? ':' : ',';
            m_sb.AppendFormat(Indent.Method + "{0} {1}(scalar)", c, fields[i]).AppendLine();
        }
        m_sb.AppendLine(Indent.Class + "{}");

        ReadOnlySpan<string> fieldsLower = FieldsLower;

        //- Field constructor
        m_sb.AppendFormat(Indent.Class + "constexpr {0}(", Type);
        //-- Params
        for (int i = 0; i < numFields; i++)
        {
            if (i != 0)
            {
                m_sb.Append(", ");
            }
            m_sb.AppendFormat("{0} {1}", FieldParam, fieldsLower[i]);
        }
        m_sb.AppendLine(")");
        //-- Initializer list
        for (int i = 0; i < numFields; i++)
        {
            char c = i == 0 ? ':' : ',';
            m_sb.AppendFormat(Indent.Method + "{0} {1}({2})", c, fields[i], fieldsLower[i]).AppendLine();
        }
        m_sb.AppendLine(Indent.Class + "{}");
    }

    private void _GenerateCompoundOperators()
    {
        m_sb.AppendLine(Indent.Class + "//- Compound");

        m_sb.AppendLine(Indent.Class + "//-- Vector | Scalar");
        WriteOperators(BaseType, true);

        m_sb.AppendLine(Indent.Class + "//-- Vector | Vector");
        WriteOperators(TypeParam, false);

        void WriteOperators(string rhsType, bool rhsIsScalar)
        {
            foreach (string op in CompoundOperators)
            {
                m_sb.AppendFormat(Indent.Class + "constexpr {0}& operator{1}({2} rhs) {{ ", Type, op, rhsType);

                foreach (string field in Fields)
                {
                    m_sb.AppendFormat("{0} {1} rhs", field, op);
                    if (rhsIsScalar == false)
                    {
                        m_sb.Append('.').Append(field);
                    }
                    m_sb.Append("; ");
                }

                m_sb.AppendLine("return *this; }");
            }
        }
    }

    private void _GenerateUnaryOperators()
    {
        m_sb.AppendLine(Indent.Class + "//- Unary");

        m_sb.AppendFormat(Indent.Class + "friend constexpr {0} operator-({1} rhs) {{ return {0}(", Type, TypeParam);

        ReadOnlySpan<string> fields = Fields;
        for (int i = 0; i < NumFields; i++)
        {
            if (i != 0)
            {
                m_sb.Append(", ");
            }
            m_sb.Append("-rhs.").Append(fields[i]);
        }
        m_sb.AppendLine("); }");
    }

    private void _GenerateBinaryOperators()
    {
        m_sb.AppendLine(Indent.Class + "//- Binary");

        m_sb.AppendFormat(Indent.Class + "//-- {0} | Scalar", TypeDesc).AppendLine();
        WriteOperators(TypeParam, BaseType, false, true);

        m_sb.AppendFormat(Indent.Class + "//-- Scalar | {0}", TypeDesc).AppendLine();
        WriteOperators(BaseType, TypeParam, true, false);

        m_sb.AppendFormat(Indent.Class + "//-- {0} | {0}", TypeDesc).AppendLine();
        WriteOperators(TypeParam, TypeParam, false, false);

        void WriteOperators(string lhsType, string rhsType, bool lhsIsScalar, bool rhsIsScalar)
        {
            ReadOnlySpan<string> fields = Fields;
            foreach (char op in BinaryOperators)
            {
                m_sb.AppendFormat(Indent.Class + "friend constexpr {0} operator{1}({2} lhs, {3} rhs) {{ return {0}(", Type, op, lhsType, rhsType);

                for (int i = 0; i < NumFields; i++)
                {
                    if (i != 0)
                    {
                        m_sb.Append(", ");
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

                    m_sb.AppendFormat(format, op, fields[i]);
                }

                m_sb.AppendLine("); }");
            }
        }
    }
}
