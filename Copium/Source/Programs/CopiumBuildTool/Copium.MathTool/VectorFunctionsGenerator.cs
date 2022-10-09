using System;
using System.Collections.Generic;
using System.Linq;

namespace Copium.MathTool;


internal sealed class VectorFunctionsGenerator : MathGenerator
{
    public const string InternalPartitionName = "VectorFunctions";


    private static readonly string[] s_HeaderImports = { "<cmath>", "<utility>" };

    private readonly List<VectorType> m_vectorTypes;
    private readonly VectorType[] m_floatVectorTypes;


    private VectorFunctionsGenerator(List<VectorType> vectorTypes)
    {
        m_vectorTypes = vectorTypes;
        m_floatVectorTypes = vectorTypes.Where(type => type.BaseTypeEnum.IsFloatType()).ToArray();
    }


    public static void Generate(string filePath, string moduleName, List<VectorType> vectorTypes)
    {
        VectorFunctionsGenerator vectorFunctionsGenerator = new(vectorTypes);
        vectorFunctionsGenerator._Generate(filePath, moduleName);
    }


    private void _Generate(string filePath, string moduleName)
    {
        Start(moduleName, _GetModuleImports(), s_HeaderImports, MathToolSettings.MathNamespace);

        _GenerateTrigonometric();
        m_sb.AppendLine()
            .AppendLine();

        _GenerateGeometric();

        End(filePath);
    }

    private void _GenerateTrigonometric()
    {
        //- Radians
        GenerateSimpleFunction(Indent.Namespace + "[[nodiscard]] constexpr {0} Radians({1} degrees) noexcept {{ return degrees * k_DegreeToRadians; }}");
        m_sb.AppendLine();
        //- Degrees
        GenerateSimpleFunction(Indent.Namespace + "[[nodiscard]] constexpr {0} Degrees({1} radians) noexcept {{ return radians * k_RadiansToDegree; }}");
        m_sb.AppendLine();

        m_sb.AppendLine(Indent.Namespace + "// NOTE(v.matushkin): Seems like compilers can't use SVML intrinsics (https://godbolt.org/z/9fdT7f3Tq)")
            .AppendLine();

        //- Sin
        GenerateTrigonometricFunction("Sin", baseType => baseType == BaseType.float32 ? "sinf" : "sin");
        m_sb.AppendLine();
        //- Cos
        GenerateTrigonometricFunction("Cos", baseType => baseType == BaseType.float32 ? "cosf" : "cos");
        m_sb.AppendLine();

        //- SinCos
        GenerateSimpleFunction(Indent.Namespace + "[[nodiscard]] std::pair<{0}, {0}> SinCos({1} v) noexcept {{ return {{ Sin(v), Cos(v) }}; }}");

        void GenerateSimpleFunction(string format)
        {
            foreach (VectorType vectorType in m_floatVectorTypes)
            {
                m_sb.AppendFormat(format, vectorType.Type, vectorType.TypeParam).AppendLine();
            }
        }

        void GenerateTrigonometricFunction(string name, Func<BaseType, string> stdFunction)
        {
            foreach (VectorType vectorType in m_floatVectorTypes)
            {
                const string format = Indent.Namespace + "[[nodiscard]] {0} {1}({2} v) noexcept {{ return {0}(";
                m_sb.AppendFormat(format, vectorType.Type, name, vectorType.TypeParam);

                int numFields = vectorType.NumFields;
                ReadOnlySpan<string> fields = vectorType.Fields;

                for (int i = 0; i < numFields; i++)
                {
                    if (i != 0)
                    {
                        m_sb.Append(", ");
                    }
                    m_sb.AppendFormat("std::{0}(v.{1})", stdFunction(vectorType.BaseTypeEnum), fields[i]);
                }

                m_sb.AppendLine("); }");
            }
        }
    }

    private void _GenerateGeometric()
    {
        //- Dot
        foreach (VectorType vectorType in m_vectorTypes)
        {
            m_sb.AppendFormat(Indent.Namespace + "[[nodiscard]] constexpr {0} Dot({1} lhs, {1} rhs) noexcept {{ return ", vectorType.BaseTypeStr, vectorType.TypeParam);

            int numFields = vectorType.NumFields;
            ReadOnlySpan<string> fields = vectorType.Fields;

            for (int i = 0; i < numFields; i++)
            {
                if (i != 0)
                {
                    m_sb.Append(" + ");
                }
                m_sb.AppendFormat("lhs.{0} * rhs.{0}", fields[i]);
            }

            m_sb.AppendLine("; }");
        }

        m_sb.AppendLine();

        foreach (VectorType vectorType in m_floatVectorTypes)
        {
            // Only generate for Float3 and Double3
            if (vectorType.NumFields == 3 && vectorType.BaseTypeEnum.IsFloatType())
            {
                m_sb.AppendFormat(Indent.Namespace + "[[nodiscard]] constexpr {0} Cross({1} lhs, {1} rhs) noexcept", vectorType.Type, vectorType.TypeParam).AppendLine()
                    .AppendLine(Indent.Namespace + "{")
                    .AppendFormat(Indent.FreeFunction + "return {0}(lhs.Y * rhs.Z - lhs.Z * rhs.Y,", vectorType.Type).AppendLine()
                    .AppendLine(Indent.FreeFunction + "              lhs.Z * rhs.X - lhs.X * rhs.Z,")
                    .AppendLine(Indent.FreeFunction + "              lhs.X * rhs.Y - lhs.Y * rhs.X);")
                    .AppendLine(Indent.Namespace + "}");
            }
        }
    }


    private IEnumerable<string> _GetModuleImports()
    {
        return m_vectorTypes.Select(type => ':' + type.Type)
            .Append(":Functions")
            .Append(":Constants")
            .Order()
            .Append("CopiumEngine.Core.CoreTypes");
    }
}
