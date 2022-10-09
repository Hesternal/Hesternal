using System;
using System.Collections.Generic;

namespace Copium.MathTool;


internal sealed class MatrixGenerator : MathTypeGenerator
{
    private readonly string m_moduleImport;


    public static void Generate(string filePath, string moduleName, MatrixType matrixType)
    {
        MatrixGenerator matrixGenerator = new(matrixType);
        matrixGenerator.Generate(filePath, moduleName);
    }


    private MatrixGenerator(MatrixType matrixType)
        : base(matrixType)
    {
        m_moduleImport = ':' + FieldType;
    }


    protected override IEnumerable<string> ModuleImports => new[] { m_moduleImport };


    protected override void GenerateConstructors()
    {
        ReadOnlySpan<string> fields = Fields;

        //- Scalar constructor
        string argsIndent = Indent.Class + new string(' ', Type.Length + 11); // length of "constexpr " + '(' = 11

        m_sb.AppendFormat(Indent.Class + "constexpr {0}(", Type);
        //-- Params
        for (int i = 0; i < Columns; i++)
        {
            if (i != 0)
            {
                m_sb.AppendLine(",")
                    .Append(argsIndent);
            }

            for (int j = 0; j < Rows; j++)
            {
                if (j != 0)
                {
                    m_sb.Append(", ");
                }
                m_sb.AppendFormat("{0} m{1}{2}", BaseType, j, i);
            }
        }
        m_sb.AppendLine(")");
        //-- Initializer list
        for (int i = 0; i < Columns; i++)
        {
            char c = i == 0 ? ':' : ',';
            m_sb.AppendFormat(Indent.Method + "{0} {1}(", c, fields[i]);

            for (int j = 0; j < Rows; j++)
            {
                if (j != 0) m_sb.Append(", ");
                m_sb.AppendFormat("m{0}{1}", j, i);
            }

            m_sb.AppendLine(")");
        }
        m_sb.AppendLine(Indent.Class + "{}");
    }

    protected override void GenerateStaticMethods()
    {
        string argsIndent = Indent.Method + new string(' ', Type.Length + 8); // length of "return " + '(' = 8

        //- Zero
        string zero = BaseTypeEnum.ToValueLiteral(0);
        m_sb.AppendFormat(Indent.Class + "[[nodiscard]] static constexpr {0} Zero() noexcept {{ return {0}({1}); }}", Type, zero).AppendLine();

        //- Identity
        if (Rows == Columns)
        {
            string one = BaseTypeEnum.ToValueLiteral(1);

            m_sb.AppendFormat(Indent.Class + "[[nodiscard]] static constexpr {0} Identity() noexcept", Type).AppendLine()
                .AppendLine(Indent.Class + "{")
                .AppendFormat(Indent.Method + "return {0}(", Type);

            for (int i = 0; i < Columns; i++)
            {
                if (i != 0)
                {
                    m_sb.AppendLine(",")
                        .Append(argsIndent);
                }

                for (int j = 0; j < Rows; j++)
                {
                    if (j != 0)
                    {
                        m_sb.Append(", ");
                    }
                    m_sb.Append(i == j ? one : zero);
                }
            }
            m_sb.AppendLine(");")
                .AppendLine(Indent.Class + "}");
        }
    }
}
