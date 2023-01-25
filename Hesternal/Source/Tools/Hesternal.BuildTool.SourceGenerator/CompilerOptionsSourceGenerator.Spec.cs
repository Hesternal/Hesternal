using System.Collections.Generic;
using System.Collections.ObjectModel;

using Microsoft.CodeAnalysis;

namespace Hesternal.BuildTool.SourceGenerator;


internal sealed partial class CompilerOptionsSourceGenerator : IIncrementalGenerator
{
    internal enum CompilerFlagType
    {
        Bool,
        Enum,
        String,
    }

    internal enum OptionsType
    {
        Compiler,
        Linker,
    }


    internal class CompilerFlagSpec
    {
        public readonly CompilerFlagType Type;
        public readonly bool Nullable;
        public readonly string Name;
        public readonly string? VcxprojName;
        public readonly string? Default;
        public readonly string? Description;


        public CompilerFlagSpec(CompilerFlagType type, bool nullable, string name, string? vcxprojName, string? @default, string? description = null)
        {
            Type = type;
            Nullable = nullable;
            Name = name;
            VcxprojName = vcxprojName;
            Default = @default;
            Description = description;
        }
    }


    internal sealed class BoolFlagSpec : CompilerFlagSpec
    {
        public readonly string? Flag;
        public readonly string? ReverseFlag;


        public BoolFlagSpec(bool nullable, string name, string? vcxprojName, string? @default, string? flag = null, string? reverseFlag = null, string? description = null)
            : base(CompilerFlagType.Bool, nullable, name, vcxprojName, @default, description)
        {
            Flag = flag;
            ReverseFlag = reverseFlag;
        }
    }


    internal sealed class EnumFlagSpec : CompilerFlagSpec
    {
        public sealed class Value
        {
            public readonly string Name;
            public readonly string? VcxprojName;
            public readonly string? Flag;
            public readonly string? Description;


            public Value(string name, string? vcxprojName, string? flag, string? description = null)
            {
                Name = name;
                VcxprojName = vcxprojName;
                Flag = flag;
                Description = description;
            }
        }


        private readonly List<Value> m_values;

        public readonly ReadOnlyCollection<Value> Values;


        public EnumFlagSpec(bool nullable, string name, string? vcxprojName, string? @default, List<Value> values, string? description = null)
            : base(CompilerFlagType.Enum, nullable, name, vcxprojName, @default, description)
        {
            m_values = values;
            Values = m_values.AsReadOnly();
        }
    }


    internal sealed class StringFlagSpec : CompilerFlagSpec
    {
        public readonly string? Flag;


        public StringFlagSpec(string name, string? vcxprojName, string? flag = null, string? description = null)
            : base(CompilerFlagType.String, true, name, vcxprojName, null, description)
        {
            Flag = flag;
        }
    }


    internal sealed class OptionsSpec
    {
        public readonly string Name;
        public readonly OptionsType Type;
        public readonly List<CompilerFlagSpec> Flags = new();


        private OptionsSpec(OptionsType type, string name)
        {
            Name = name;
            Type = type;
        }


        public static OptionsSpec Compiler(string name) => new(OptionsType.Compiler, name);
        public static OptionsSpec Linker(string name) => new(OptionsType.Linker, name);
    }
}
