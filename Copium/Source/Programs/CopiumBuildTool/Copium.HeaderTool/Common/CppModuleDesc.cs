using System.Collections.Generic;
using System.IO;

namespace Copium.HeaderTool
{
    internal sealed class CppEnumDesc
    {
        public sealed class CppEnumVariable
        {
            public readonly string Name;
            public readonly string ValueStr;

            public CppEnumVariable(string name, string valueStr)
            {
                Name = name;
                ValueStr = valueStr;
            }
        }

        public readonly CppEnumType Type;
        public readonly List<CppEnumVariable> Variables = new List<CppEnumVariable>();
        /// <summary>
        /// Line with a CHT_ENUM macro
        /// </summary>
        public readonly int MacroLine;


        public CppEnumDesc(CppEnumType type, int macroLine)
        {
            Type = type;
            MacroLine = macroLine;
        }
    }


    internal sealed class CppPropertyDesc
    {
        public readonly string Name;
        public readonly CppType Type;


        public CppPropertyDesc(string name, CppType type)
        {
            Name = name;
            Type = type;
        }
    }


    internal sealed class CppStructDesc
    {
        public readonly CppType Type;
        /// <summary>
        /// Struct field declared with a CHT_PROPERTY macro
        /// </summary>
        public readonly List<CppPropertyDesc> Properties = new List<CppPropertyDesc>();
        /// <summary>
        /// Line with a CHT_GENERATED_BODY macro
        /// </summary>
        public readonly int GeneratedBodyMacroLine;


        public CppStructDesc(CppType type, int generatedBodyMacroLine)
        {
            Type = type;
            GeneratedBodyMacroLine = generatedBodyMacroLine;
        }
    }


    internal sealed class CppHeaderDesc
    {
        public readonly FileInfo HeaderFile;
        /// <summary>
        /// Module declaration, i.e. for 'export module A.B.C;' Module will be "A.B.C"
        /// </summary>
        public readonly string Module;
        /// <summary>
        /// Enums declared with CHT_ENUM macro
        /// </summary>
        public readonly List<CppEnumDesc> Enums;
        /// <summary>
        /// Structs declared with CHT_STRUCT macro
        /// </summary>
        public readonly List<CppStructDesc> Structs;


        public CppHeaderDesc(FileInfo headerFile, string module, List<CppEnumDesc> enums, List<CppStructDesc> structs)
        {
            HeaderFile = headerFile;
            Module = module;
            Enums = enums;
            Structs = structs;
        }
    }
}
