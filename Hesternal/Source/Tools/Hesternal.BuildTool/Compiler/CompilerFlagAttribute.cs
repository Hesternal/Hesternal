using System;

namespace Hesternal.BuildTool;


public static class CompilerFlag
{
    // NOTE(v.matushkin): May be it's better to get flag info from xml comment,
    //   so there will be no need to repeat the same flag names for this attributes,
    //   but at the same time seems like retrieving flag info from comment will be harder?
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public sealed class BoolAttribute : Attribute
    {
        public BoolAttribute(string? trueFlag = null, string? falseFlag = null)
        {
        }
    }


    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public sealed class StringAttribute : Attribute
    {
        public StringAttribute(string? flag)
        {
        }
    }


    [AttributeUsage(AttributeTargets.Enum, AllowMultiple = false)]
    public sealed class EnumAttribute : Attribute
    {
        public EnumAttribute()
        {
        }
    }


    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public sealed class EnumOptionAttribute : Attribute
    {
        public EnumOptionAttribute(string? flag = null, string? vcxprojName = null)
        {
        }
    }
}
