namespace Copium.HeaderTool.Generator
{
    internal static class ClassGenerator
    {
        public static void WriteHpp(CppHeaderDesc cppHeaderDesc, CppWriter cppWriter, string chtFilePathMacro)
        {
            foreach (CppClassDesc cppClassDesc in cppHeaderDesc.Classes)
            {
                // #define ${ChtFilePathMacro}_${GeneratedBodyMacroLine} \
                cppWriter.Write("#define ");
                cppWriter.Write(chtFilePathMacro);
                cppWriter.Write('_');
                cppWriter.Write(cppClassDesc.GeneratedBodyMacroLine);

                if (cppClassDesc.HasFlag(ClassFlags.Abstract))
                {
                    cppWriter.WriteLine();
                    continue;
                }

                cppWriter.WriteLine(@" \");

                // typedef BaseClass Super;\
                if (string.IsNullOrEmpty(cppClassDesc.BaseClass) == false)
                {
                    cppWriter.Write("    typedef ");
                    cppWriter.Write(cppClassDesc.BaseClass);
                    cppWriter.WriteLine(@" Super\");
                }

                cppWriter.WriteLine(@"public:\");

                // _CHT_DECLARE_CONVERTER(${ClassName})\
                cppWriter.Write("    _CHT_DECLARE_CONVERTER(");
                cppWriter.Write(cppClassDesc.Type.BaseType);
                cppWriter.WriteLine(@")\");

                cppWriter.WriteLine(@"private:\");
            }
        }

        public static void WriteCpp(CppHeaderDesc cppHeaderDesc, CppWriter cppWriter)
        {
            foreach (CppClassDesc cppClassDesc in cppHeaderDesc.Classes)
            {
                if (cppClassDesc.HasFlag(ClassFlags.Abstract))
                {
                    continue;
                }

                cppWriter.Write("IBinaryConverter& operator<<(IBinaryConverter& bc, ");
                cppWriter.Write(cppClassDesc.Type.BaseType);
                cppWriter.WriteLine("& value)");

                cppWriter.BeginBlock();
                foreach (CppPropertyDesc cppPropertyDesc in cppClassDesc.Properties)
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
