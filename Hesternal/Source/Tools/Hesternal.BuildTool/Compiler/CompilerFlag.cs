namespace Hesternal.BuildTool;


internal abstract class FlagDesc
{
    public readonly string? VcxprojName;


    protected FlagDesc(string? vcxprojName)
    {
        VcxprojName = vcxprojName;
    }
}


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
