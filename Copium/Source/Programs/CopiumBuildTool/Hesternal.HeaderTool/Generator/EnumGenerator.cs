namespace Copium.HeaderTool.Generator
{
    internal static class EnumGenerator
    {
        private static EnumGeneratorConfig s_Config;

        public static string[] HppIncludes => s_Config.Hpp.Includes;

        public static string[] CppStdIncludes => s_Config.Cpp.StdIncludes;
        public static string[] CppImports => s_Config.Cpp.Imports;


        public static void SetConfig(EnumGeneratorConfig config) => s_Config = config;


        public static void WriteHpp(CppHeaderDesc cppHeaderDesc, CppWriter cppWriter, string chtFilePathMacro)
        {
            foreach (CppEnumDesc cppEnumDesc in cppHeaderDesc.Enums)
            {
                //- Macro name
                cppWriter.Write("#define ");
                cppWriter.Write(chtFilePathMacro);
                cppWriter.Write('_');
                cppWriter.Write(cppEnumDesc.MacroLine);
                cppWriter.WriteLine(@" \");

                //- Macro body
                //-- Enum forward declaration
                cppWriter.Write("    enum class ");
                cppWriter.Write(cppEnumDesc.Type.BaseType);
                cppWriter.Write(" : ");
                cppWriter.Write(cppEnumDesc.Type.UnderlyingType);
                cppWriter.WriteLine(@"; \");
                //-- Enum type trait
                cppWriter.Write("    template<> struct IsChtEnum<");
                cppWriter.Write(cppEnumDesc.Type.BaseType);
                cppWriter.WriteLine("> : std::true_type {};");
            }
        }

        public static void WriteCpp(CppHeaderDesc cppHeaderDesc, CppWriter cppWriter)
        {
            foreach (CppEnumDesc cppEnumDesc in cppHeaderDesc.Enums)
            {
                _WriteEnumToStringFunction(cppWriter, cppEnumDesc);
                _WriteEnumFromStringFunction(cppWriter, cppEnumDesc);
            }
        }


        private static void _WriteEnumToStringFunction(CppWriter cppWriter, CppEnumDesc cppEnumDesc)
        {
            string enumName = cppEnumDesc.Type.BaseType;
            string enumNameVariable = string.Concat(char.ToLower(enumName[0]).ToString(), enumName.Substring(1));
            int numEnumVariables = cppEnumDesc.Variables.Count;

            // template<>
            // std::string_view EnumUtils::ToString([[maybe_unused]] EnumName enumName)
            cppWriter.WriteLine("template<>");
            cppWriter.Write("std::string_view EnumUtils::ToString(");
            if (numEnumVariables == 1)
            {
                cppWriter.Write("[[maybe_unused]] ");
            }
            cppWriter.Write(enumName);
            cppWriter.Write(' ');
            cppWriter.Write(enumNameVariable);
            cppWriter.WriteLine(')');

            cppWriter.BeginBlock();

            if (numEnumVariables < s_Config.Cpp.ToStringMapThreshold)
            {
                for (int i = 0; i < numEnumVariables - 1; ++i)
                {
                    var enumVariable = cppEnumDesc.Variables[i];

                    // if (enumName == EnumName::EnumVariableName) return "EnumVariableName";
                    cppWriter.Write("if (");
                    cppWriter.Write(enumNameVariable);
                    cppWriter.Write(" == ");
                    cppWriter.Write(enumName);
                    cppWriter.Write("::");
                    cppWriter.Write(enumVariable.Name);
                    cppWriter.Write(") return \"");
                    cppWriter.Write(enumVariable.Name);
                    cppWriter.WriteLine("\";");
                }
                // return "LastEnumVariableName";
                cppWriter.Write("return \"");
                cppWriter.Write(cppEnumDesc.Variables[cppEnumDesc.Variables.Count - 1].Name);
                cppWriter.WriteLine("\";");
            }
            else
            {
                // static const std::string_view enumNameToString[]
                cppWriter.Write("static const std::string_view ");
                cppWriter.Write(enumNameVariable);
                cppWriter.WriteLine("ToString[]");

                cppWriter.BeginInitializerList();
                foreach (var enumVariable in cppEnumDesc.Variables)
                {
                    // "EnumVariableName",
                    cppWriter.Write('"');
                    cppWriter.Write(enumVariable.Name);
                    cppWriter.WriteLine("\",");
                }
                cppWriter.EndInitializerList();

                // return enumNameToString[std::to_underlying(enumName)];
                cppWriter.Write("return ");
                cppWriter.Write(enumNameVariable);
                cppWriter.Write("ToString[std::to_underlying(");
                cppWriter.Write(enumNameVariable);
                cppWriter.WriteLine(")];");
            }

            cppWriter.EndBlock();
        }

        private static void _WriteEnumFromStringFunction(CppWriter cppWriter, CppEnumDesc cppEnumDesc)
        {
            string enumName = cppEnumDesc.Type.BaseType;
            int numEnumVariables = cppEnumDesc.Variables.Count;

            // template<>
            // EnumName EnumUtils::FromString([[maybe_unused]] std::string_view str)
            cppWriter.WriteLine("template<>");
            cppWriter.Write(enumName);
            cppWriter.Write(" EnumUtils::FromString(");
            if (numEnumVariables == 1)
            {
                cppWriter.Write("[[maybe_unused]] ");
            }
            cppWriter.WriteLine("std::string_view str)");

            cppWriter.BeginBlock();

            if (numEnumVariables < s_Config.Cpp.FromStringMapThreshold)
            {
                for (int i = 0; i < numEnumVariables - 1; ++i)
                {
                    var enumVariable = cppEnumDesc.Variables[i];

                    // if (str == "EnumVariableName") return EnumName::EnumVariableName;
                    cppWriter.Write("if (str == \"");
                    cppWriter.Write(enumVariable.Name);
                    cppWriter.Write("\") return ");
                    cppWriter.Write(enumName);
                    cppWriter.Write("::");
                    cppWriter.Write(enumVariable.Name);
                    cppWriter.WriteLine(';');
                }
                // return EnumName::LastEnumVariableName;
                cppWriter.Write("return ");
                cppWriter.Write(enumName);
                cppWriter.Write("::");
                cppWriter.Write(cppEnumDesc.Variables[cppEnumDesc.Variables.Count - 1].Name);
                cppWriter.WriteLine(';');
            }
            else
            {
                // static const std::unordered_map<std::string_view, EnumName> stringToEnumName
                cppWriter.Write("static const std::unordered_map<std::string_view, ");
                cppWriter.Write(cppEnumDesc.Type.BaseType);
                cppWriter.Write("> stringTo");
                cppWriter.WriteLine(enumName);

                cppWriter.BeginInitializerList();
                foreach (var enumVariable in cppEnumDesc.Variables)
                {
                    // {"EnumName", EnumName::EnumVariableName},
                    cppWriter.Write("{\"");
                    cppWriter.Write(enumVariable.Name);
                    cppWriter.Write("\", ");
                    cppWriter.Write(enumName);
                    cppWriter.Write("::");
                    cppWriter.Write(enumVariable.Name);
                    cppWriter.WriteLine("},");
                }
                cppWriter.EndInitializerList();

                // return stringToEnumName.find(str)->second;
                cppWriter.Write("return stringTo");
                cppWriter.Write(enumName);
                cppWriter.WriteLine(".find(str)->second;");
            }

            cppWriter.EndBlock();
        }
    }
}
