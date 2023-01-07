using System;

namespace Copium.BuildTool;


public interface ICompilerOptions
{
}


[AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
public sealed class SupportedCompilersAttribute : Attribute
{
    public SupportedCompilersAttribute(params Type[] supportedCompilers)
    {
    }
}
