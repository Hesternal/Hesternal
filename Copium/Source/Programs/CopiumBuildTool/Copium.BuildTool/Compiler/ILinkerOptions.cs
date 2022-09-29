using System;

namespace Copium.BuildTool;


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
