namespace Hesternal.BuildTool.SourceGenerator;


internal static partial class HesternalInfo
{
    public static class Attribute
    {
        public static class CompilerFlag
        {
            private const string k_BaseClass = "CompilerFlag.";
            private const string k_FullyQualifiedBaseClass = "Hesternal.BuildTool.CompilerFlag+";


            public static class Enum
            {
                public const string ShortName = k_BaseClass + "Enum";
                public const string FullName = k_FullyQualifiedBaseClass + "EnumAttribute";
            }


            public static class EnumOption
            {
                public const string ShortName = k_BaseClass + "EnumOption";
                public const string FullName = k_FullyQualifiedBaseClass + "EnumOptionAttribute";

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


        public static class CompilerOptions
        {
            public const string FullName = "Hesternal.BuildTool.CompilerOptionsAttribute";
        }


        public static class LinkerOptions
        {
            public const string FullName = "Hesternal.BuildTool.LinkerOptionsAttribute";
        }
    }
}
