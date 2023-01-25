using System;
using System.Collections.Generic;
using System.Linq;

using Microsoft.Build.CPPTasks;
//using Microsoft.Build.FXCTask;
//using Microsoft.Build.Utilities;

namespace Dummy;


public enum FlagType
{
    Bool, Enum, String, StringList,
}

internal abstract class FlagDesc
{
    public readonly string? VcxprojName;


    protected FlagDesc(string? vcxprojName)
    {
        VcxprojName = vcxprojName;
    }
}

internal sealed class BoolFlagDesc : FlagDesc
{
    public readonly string? Flag;
    public readonly string? ReverseFlag;


    internal BoolFlagDesc(string? flag, string? reverseFlag, string? vcxprojName)
        : base(vcxprojName)
    {
        Flag = flag;
        ReverseFlag = reverseFlag;
    }


    public BoolFlag MakeFlag(bool initialValue) => new(this, initialValue);
}

internal sealed class StringFlagDesc : FlagDesc
{
    public readonly string? Flag;


    internal StringFlagDesc(string? flag, string? vcxprojName)
        : base(vcxprojName)
    {
        Flag = flag;
    }


    public StringFlag MakeFlag(string? initialValue) => new(this, initialValue);
}

internal sealed class StringListFlagDesc : FlagDesc
{
    public readonly string? Flag;


    internal StringListFlagDesc(string? flag)
        : base(null)
    {
        Flag = flag;
    }


    public StringListFlag MakeFlag() => new(this);
}

//internal class EnumFlagDesc : FlagDesc
//{
//    public readonly EnumFlag.Option Default;
//
//
//    internal EnumFlagDesc(EnumFlag.Option @default, string? vcxprojName)
//        : base(vcxprojName)
//    {
//        Default = @default;
//    }
//}


public abstract class CompilerFlag
{
    public abstract string? Flag { get; }
    public readonly string? VcxprojName;
    public abstract string? VcxprojValue { get; }

    protected CompilerFlag(string? vcxprojName)
    {
        VcxprojName = vcxprojName;
    }
}

public sealed class BoolFlag : CompilerFlag
{
    private static readonly string s_VcxprojValueFalse = "false";
    private static readonly string s_VcxprojValueTrue = "true";

    private readonly BoolFlagDesc m_desc;
    private string? m_flag;
    private bool m_value;
    private string? m_vcxprojValue;


    public bool Value
    {
        get => m_value;
        set
        {
            m_value = value;
            if (value)
            {
                m_flag = m_desc.Flag;
                m_vcxprojValue = s_VcxprojValueTrue;
            }
            else
            {
                m_flag = m_desc.ReverseFlag;
                m_vcxprojValue = s_VcxprojValueFalse;
            }
        }
    }
    public override string? Flag => m_flag;
    public override string? VcxprojValue => m_vcxprojValue;


    internal BoolFlag(BoolFlagDesc desc, bool initialValue)
        : base(desc.VcxprojName)
    {
        m_desc = desc;
        Value = initialValue;
    }
}

public sealed class StringFlag : CompilerFlag
{
    private readonly StringFlagDesc m_desc;
    private string? m_flag;
    private string? m_value;
    private string? m_vcxprojValue;


    public string? Value
    {
        get => m_value;
        set
        {
            m_value = value;
            m_vcxprojValue = value;
            m_flag = value is null ? null : m_desc.Flag + value;
        }
    }
    public override string? Flag => m_flag;
    public override string? VcxprojValue => m_vcxprojValue;


    internal StringFlag(StringFlagDesc desc, string? initialValue)
        : base(desc.VcxprojName)
    {
        m_desc = desc;
        Value = initialValue;
    }
}

public sealed class StringListFlag : CompilerFlag
{
    private readonly StringListFlagDesc m_desc;
    private List<string> m_value = new();


    public List<string> Value => m_value;

    // NOTE(v.matushkin): The way this Flag property works right now is very questionable
    //   and probably will not work for a lot of situations
    public override string? Flag => m_value.Count != 0
        ? string.Join(' ', m_value.Where(_NotEmptyString).Select(str => m_desc.Flag + str))
        : null;

    // NOTE(v.matushkin): Can be optimized, but since I give an easy access to m_value there is no way
    //   to actually tell if List was changed or not.
    public override string? VcxprojValue => m_value.Count != 0
        ? string.Join(';', m_value.Where(_NotEmptyString))
        : null;


    internal StringListFlag(StringListFlagDesc desc)
        : base(desc.VcxprojName)
    {
        m_desc = desc;
    }


    private static bool _NotEmptyString(string str) => string.IsNullOrWhiteSpace(str) == false;
}

public abstract class EnumFlag : CompilerFlag
{
    private readonly string? m_flag;
    private readonly string? m_vcxprojValue;


    public override string? Flag => m_flag;
    public override string? VcxprojValue => m_vcxprojValue;


    protected EnumFlag(string? flag, string vcxprojName, string? vcxprojValue)
        : base(vcxprojName)
    {
        m_flag = flag;
        m_vcxprojValue = vcxprojValue;
    }
}



public static class Msvc
{
    private static readonly BoolFlagDesc s_ScanSourceForModuleDependencies = new("/scanDependencies", null, "ScanSourceForModuleDependencies");
    public static BoolFlag ScanSourceForModuleDependencies(bool initialValue) => s_ScanSourceForModuleDependencies.MakeFlag(initialValue);


    private static readonly StringFlagDesc s_EntryPointSymbol = new("/ENTRY:", "EntryPointSymbol");
    public static StringFlag EntryPointSymbol(string? initialValue) => s_EntryPointSymbol.MakeFlag(initialValue);


    private static readonly StringListFlagDesc s_DisableSpecificWarnings = new("/wd");
    public static StringListFlag DisableSpecificWarnings() => s_DisableSpecificWarnings.MakeFlag();


    public sealed class DebugInformationFormat : EnumFlag
    {
        private DebugInformationFormat(string? flag, string? vcxprojValue)
            : base(flag, "DebugInformationFormat", vcxprojValue)
        {
        }


        public static readonly DebugInformationFormat None = new(null, "None");
        public static readonly DebugInformationFormat C7Compatible = new("/Z7", "C7Compatible");
        public static readonly DebugInformationFormat ProgramDatabase = new("/Zi", "ProgramDatabase");
        public static readonly DebugInformationFormat ProgramDatabaseForEditAndContinue = new("/ZI", "ProgramDatabaseForEditAndContinue");
    }
}


public interface ICompilerOptions
{
}

public sealed class MsvcOptions : ICompilerOptions
{
    private readonly CompilerFlag[] m_flags = new CompilerFlag[]
    {
        Msvc.ScanSourceForModuleDependencies(false),
        Msvc.EntryPointSymbol("main"),
        Msvc.DisableSpecificWarnings(),
        Msvc.DebugInformationFormat.None,
    };


    public ReadOnlySpan<CompilerFlag> AllFlags => m_flags;


    public BoolFlag ScanSourceForModuleDependencies => (BoolFlag)m_flags[0];
    public StringFlag EntryPointSymbol => (StringFlag)m_flags[1];
    public StringListFlag DisableSpecificWarnings => (StringListFlag)m_flags[2];
    public Msvc.DebugInformationFormat DebugInformationFormat
    {
        get => (Msvc.DebugInformationFormat)m_flags[3];
        set => m_flags[3] = value;
    }
}



internal static class Program
{
    private static void Main(string[] args)
    {
        MsvcOptions msvcOptions = new();
        msvcOptions.ScanSourceForModuleDependencies.Value = true;
        msvcOptions.EntryPointSymbol.Value = "main";
        msvcOptions.DisableSpecificWarnings.Value.Add("4517");
        msvcOptions.DebugInformationFormat = Msvc.DebugInformationFormat.ProgramDatabase;

        Console.WriteLine("Hello, World!");
    }
}
