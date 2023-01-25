using System;

namespace Hesternal.BuildTool;


public interface ILinkerOptions
{
}


[AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
public sealed class SupportedLinkersAttribute : Attribute
{
    public SupportedLinkersAttribute(params Type[] supportedLinkers)
    {
    }
}


[AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
public sealed class LinkerOptionsAttribute : Attribute
{
    public LinkerOptionsAttribute(bool supportsVcxproj)
    {
    }
}
