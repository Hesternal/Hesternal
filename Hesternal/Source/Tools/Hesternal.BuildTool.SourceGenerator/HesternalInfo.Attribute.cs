namespace Hesternal.BuildTool.SourceGenerator;


internal static partial class HesternalInfo
{
    public static class Attribute
    {
        public static class CompilerFlag
        {
            private const string k_CompilerFlagAttributeBaseName = "Hesternal.BuildTool.CompilerFlag+";


            public static class Enum
            {
                public const string Name = "EnumAttribute";
                public const string FullName = k_CompilerFlagAttributeBaseName + Name;
            }


            public static class EnumOption
            {
                public const string Name = "EnumOptionAttribute";
                public const string FullName = k_CompilerFlagAttributeBaseName + Name;

                public const int NumConstructorArguments = 2;
                public const int FlagArgumentIndex = 0;
                public const int VcxprojNameArgumentIndex = 1;
            }
        }


        public static class SupportedBuildSystems
        {
            public const string Name = "SupportedBuildSystemsAttribute";
        }


        public static class SupportedCompilers
        {
            public const string Name = "SupportedCompilersAttribute";
        }


        public static class SupportedLinkers
        {
            public const string Name = "SupportedLinkersAttribute";
        }
    }
}
