namespace Hesternal.BuildTool;


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
