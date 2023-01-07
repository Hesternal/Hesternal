namespace Hesternal.BuildTool;


public static class MSBuild
{
    public enum PlatformToolset
    {
        v143,
    }

    public enum PreferredToolArchitecture
    {
        Default, // NOTE(v.matushkin): Empty vcxproj value
        /// <summary>
        /// 32-bit (x86)
        /// </summary>
        x86,
        /// <summary>
        /// 64-bit (x64)
        /// </summary>
        x64,
    }

    public enum CharacterSet
    {
        NotSet,
        /// <summary>
        /// Use Unicode Character Set
        /// </summary>
        Unicode,
        /// <summary>
        /// Use Multi-Byte Character Set
        /// </summary>
        MultiByte,
    }
}
