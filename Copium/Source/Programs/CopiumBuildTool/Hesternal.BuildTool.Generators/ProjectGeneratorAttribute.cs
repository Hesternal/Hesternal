using System;

namespace Hesternal.BuildTool.Generators;


[AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
public sealed class ProjectGeneratorAttribute : Attribute
{
    public ProjectGeneratorAttribute()
    {
    }
}
