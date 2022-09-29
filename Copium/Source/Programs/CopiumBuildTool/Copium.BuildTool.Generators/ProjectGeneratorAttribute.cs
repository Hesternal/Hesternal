using System;

namespace Copium.BuildTool.Generators;


[AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
public sealed class ProjectGeneratorAttribute : Attribute
{
    public ProjectGeneratorAttribute()
    {
    }
}
