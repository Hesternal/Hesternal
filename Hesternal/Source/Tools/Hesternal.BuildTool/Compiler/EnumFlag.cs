namespace Hesternal.BuildTool;


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
