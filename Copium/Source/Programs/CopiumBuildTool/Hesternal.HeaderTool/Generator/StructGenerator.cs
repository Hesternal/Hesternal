namespace Copium.HeaderTool.Generator
{
    internal static class StructGenerator
    {
        private static StructGeneratorConfig s_Config;

        public static string[] HppImports => s_Config.Hpp.Imports;

        public static string[] CppImports => s_Config.Cpp.Imports;


        public static void SetConfig(StructGeneratorConfig config) => s_Config = config;


        public static void WriteHpp(CppHeaderDesc cppHeaderDesc, CppWriter cppWriter, string chtFilePathMacro)
        {
            foreach (CppStructDesc cppStructDesc in cppHeaderDesc.Structs)
            {
                // #define ${ChtFilePathMacro}_${GeneratedBodyMacroLine} \
                cppWriter.Write("#define ");
                cppWriter.Write(chtFilePathMacro);
                cppWriter.Write('_');
                cppWriter.Write(cppStructDesc.GeneratedBodyMacroLine);
                cppWriter.WriteLine(@" \");

                // _CHT_DECLARE_CONVERTER(${StructName})\
                cppWriter.Write("    _CHT_DECLARE_CONVERTER(");
                cppWriter.Write(cppStructDesc.Type.BaseType);
                cppWriter.WriteLine(')');
            }
        }

        public static void WriteCpp(CppHeaderDesc cppHeaderDesc, CppWriter cppWriter)
        {
            foreach (CppStructDesc cppStructDesc in cppHeaderDesc.Structs)
            {
                cppWriter.Write("IBinaryConverter& operator<<(IBinaryConverter& bc, ");
                cppWriter.Write(cppStructDesc.Type.BaseType);
                cppWriter.WriteLine("& value)");

                cppWriter.BeginBlock();
                foreach (CppPropertyDesc cppPropertyDesc in cppStructDesc.Properties)
                {
                    cppWriter.Write("bc << value.");
                    cppWriter.Write(cppPropertyDesc.Name);
                    cppWriter.WriteLine(';');
                }
                cppWriter.WriteLine("return bc;");
                cppWriter.EndBlock();
            }
        }
    }
}
