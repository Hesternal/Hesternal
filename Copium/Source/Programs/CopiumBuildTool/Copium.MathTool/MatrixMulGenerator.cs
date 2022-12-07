using System;
using System.Collections.Generic;
using System.Linq;

namespace Copium.MathTool;


// NOTE(v.matushkin): This generator is a fucking pile of shit,
//  using MathType abstraction mostly made things harder than helped me.


internal sealed class MatrixMulGenerator : MathGenerator
{
    public const string InternalPartitionName = "Mul";


    private class MulInfo
    {
        public required string ReturnType { get; init; }
        public required MathType LhsType { get; init; }
        public required MathType RhsType { get; init; }
    }

    private readonly List<MulInfo> m_mulInfos = new();
    private readonly string[] m_moduleImports;


    private MatrixMulGenerator(IEnumerable<MathType> mathTypes)
    {
        HashSet<string> uniqueTypes = new();

        foreach (var group in mathTypes.GroupBy(mathType => mathType.BaseTypeEnum))
        {
            MathType[] mathTypesGroup = group.OrderBy(type => type.Columns).ThenBy(type => type.Columns)
                .ToArray();

            for (int i = 0; i < mathTypesGroup.Length; i++)
            {
                MathType lhsType = mathTypesGroup[i];
                for (int j = 0; j < mathTypesGroup.Length; j++)
                {
                    MathType rhsType = mathTypesGroup[j];

                    if ((lhsType.IsVector && rhsType.IsVector) == false
                        && lhsType.Columns == rhsType.GetRowsOrDimensionIfVector())
                    {
                        AddMulInfo(lhsType, rhsType);
                    }
                }
            }
        }

        m_moduleImports = uniqueTypes.Select(type => ':' + type).ToArray();
        Array.Sort(m_moduleImports);

        void AddMulInfo(MathType lhsType, MathType rhsType)
        {
            string returnType = lhsType.BaseTypeEnum.ToMathType(lhsType.GetRowsOrOneIfVector(), rhsType.GetColumnsOrOneIfVector());
            m_mulInfos.Add(new() { ReturnType = returnType, LhsType = lhsType, RhsType = rhsType });

            uniqueTypes.Add(returnType);
            uniqueTypes.Add(lhsType.Type);
            uniqueTypes.Add(rhsType.Type);
        }
    }


    public static void Generate(string filePath, string moduleName, IEnumerable<MathType> mathTypes)
    {
        MatrixMulGenerator vectorFunctionsGenerator = new(mathTypes);
        vectorFunctionsGenerator._Generate(filePath, moduleName);
    }


    private void _Generate(string filePath, string moduleName)
    {
        Start(moduleName, m_moduleImports, null, MathToolSettings.MathNamespace);

        foreach (MulInfo mulInfo in m_mulInfos)
        {
            _GenerateMul(mulInfo);
        }

        End(filePath);
    }

    private void _GenerateMul(MulInfo mulInfo)
    {
        string returnType = mulInfo.ReturnType;
        MathType lhsType = mulInfo.LhsType;
        MathType rhsType = mulInfo.RhsType;

        m_cb.AppendFormat(Indent.Namespace + "[[nodiscard]] constexpr {0} Mul({1} lhs, {2} rhs) noexcept", returnType, lhsType.TypeParam, rhsType.TypeParam).AppendLine()
            .AppendLine(Indent.Namespace + "{")
            .AppendFormat(Indent.FreeFunction + "return {0}(", returnType);

        string argsIndent = Indent.FreeFunction + new string(' ', returnType.Length + 8); // length of "return " + '(' = 8

        bool rhsIsMatrix = rhsType.IsMatrix;
        int rhsDimension = rhsType.GetColumnsOrOneIfVector();

        ReadOnlySpan<string> vectorFields = MathToolSettings.VectorFields;

        for (int i = 0; i < rhsDimension; i++)
        {
            if (i != 0)
            {
                m_cb.AppendLine(",")
                    .Append(argsIndent);
            }

            for (int j = 0; j < lhsType.Columns; j++)
            {
                if (j != 0)
                {
                    m_cb.Append(" + ");
                }

                m_cb.Append("lhs.").Append(lhsType.Fields[j]).Append(" * rhs.");

                if (rhsIsMatrix)
                {
                    m_cb.Append(rhsType.Fields[i]).Append('.').Append(vectorFields[j]);
                }
                else
                {
                    m_cb.Append(rhsType.Fields[j]);
                }
            }
        }

        m_cb.AppendLine(");")
            .AppendLine(Indent.Namespace + "}")
            .AppendLine();
    }
}
