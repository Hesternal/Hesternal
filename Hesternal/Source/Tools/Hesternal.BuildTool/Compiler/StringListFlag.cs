using System.Collections.Generic;
using System.Linq;

namespace Hesternal.BuildTool;


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
