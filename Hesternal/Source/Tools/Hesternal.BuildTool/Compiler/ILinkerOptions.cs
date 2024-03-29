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
