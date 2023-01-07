using System;

namespace Hesternal.BuildTool;


public interface IBuildSystemOptions
{
}


[AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
public sealed class SupportedBuildSystemsAttribute : Attribute
{
    public SupportedBuildSystemsAttribute(params Type[] supportedBuildSystems)
    {
    }
}
