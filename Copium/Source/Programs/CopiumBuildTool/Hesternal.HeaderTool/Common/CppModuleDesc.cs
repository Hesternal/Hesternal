using System;
using System.Collections.Generic;
using System.IO;

namespace Hesternal.HeaderTool
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
        /// Struct fields declared with a CHT_PROPERTY macro
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


    [Flags]
    enum ClassFlags : uint
    {
        None     = 0,
        Abstract = 1 << 0,
    }

    // NOTE(v.matushkin): I'm not sure how should I handle this struct/class thing, they are identical, at least for now.
    //  Completely different classes better for the type safety, since I handle struc/class as a different entities.
    //  But may the thing is that I shouldn't and should unify them somehow.
    internal sealed class CppClassDesc
    {
        public readonly CppType Type;
        public readonly string BaseClass;
        public readonly ClassFlags Flags;
        /// <summary>
        /// Class fields declared with a CHT_PROPERTY macro
        /// </summary>
        public readonly List<CppPropertyDesc> Properties = new List<CppPropertyDesc>();
        /// <summary>
        /// Line with a CHT_GENERATED_BODY macro
        /// </summary>
        public readonly int GeneratedBodyMacroLine;


        public CppClassDesc(CppType type, string baseClass, ClassFlags flags, int generatedBodyMacroLine)
        {
            Type = type;
            BaseClass = baseClass;
            Flags = flags;
            GeneratedBodyMacroLine = generatedBodyMacroLine;
        }


        public bool HasFlag(ClassFlags flag) => (Flags & flag) != 0;
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
        /// <summary>
        /// Classes declared with CHT_CLASS macro
        /// </summary>
        public readonly List<CppClassDesc> Classes;

        public readonly bool HasEnums;
        public readonly bool HasStructs;
        public readonly bool HasClasses;
        public readonly bool HasStructsOrClasses;


        public CppHeaderDesc(
            FileInfo headerFile, string module, List<CppEnumDesc> enums, List<CppStructDesc> structs, List<CppClassDesc> classes
            )
        {
            HeaderFile = headerFile;
            Module = module;
            Enums = enums;
            Structs = structs;
            Classes = classes;

            HasEnums = Enums.Count != 0;
            HasStructs = Structs.Count != 0;
            HasClasses = Classes.Count != 0;
            HasStructsOrClasses = HasStructs || HasClasses;
        }
    }
}
