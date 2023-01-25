namespace Hesternal.BuildTool;


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


public sealed class BoolFlag : CompilerFlag
{
    private static readonly string s_VcxprojValueFalse = "false";
    private static readonly string s_VcxprojValueTrue = "true";

    private readonly BoolFlagDesc m_desc;
    private string? m_flag;
    private bool? m_value;
    private string? m_vcxprojValue;


    public bool? Value
    {
        get => m_value;
        set
        {
            m_value = value;
            if (value.HasValue)
            {
                if (value.Value)
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
            else
            {
                m_flag = null;
                m_vcxprojValue = null;
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
