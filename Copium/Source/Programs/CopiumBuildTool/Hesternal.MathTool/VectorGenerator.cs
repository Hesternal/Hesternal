using System.Collections.Generic;
using System.Linq;

using Hesternal.MathTool.Cpp;

namespace Hesternal.MathTool;

/*
internal sealed class VectorGenerator : MathTypeGenerator
{
    private VectorGenerator(VectorType vectorType)
        : base(vectorType)
    {
    }


    public static void Generate(string filePath, string moduleName, VectorType vectorType)
    {
        VectorGenerator vectorGenerator = new(vectorType);
        vectorGenerator.Generate(filePath, moduleName);
    }


    protected override IEnumerable<string> ModuleImports => Enumerable.Empty<string>();


    protected override void GenerateConstructors()
    {

    }

    protected override void GenerateStaticMethods()
    {
        string zero = CppSystemType.ToValueLiteral(0);
        string one = CppSystemType.ToValueLiteral(1);
        m_cb.AppendFormat(Indent.Class + "[[nodiscard]] static constexpr {0} Zero() noexcept {{ return {0}({1}); }}", Type, zero).AppendLine();
        m_cb.AppendFormat(Indent.Class + "[[nodiscard]] static constexpr {0} One() noexcept {{ return {0}({1}); }}", Type, one).AppendLine();
    }


    private void _GenerateVectorConstructors()
    {

    }
}
*/
