namespace Copium.BuildTool.Platform;


internal sealed class CompilerFlags
{
    public sealed class PreprocessorDefines
    {
        public readonly string[] Debug;
        public readonly string[] Development;
        public readonly string[] Release;

        public PreprocessorDefines(string[] debug, string[] development, string[] release)
        {
            Debug = debug;
            Development = development;
            Release = release;
        }
    }
    public sealed class CppFlags
    {
        public readonly string[] Common;
        public readonly string[] Debug;
        public readonly string[] Development;
        public readonly string[] Release;

        public CppFlags(string[] common, string[] debug, string[] development, string[] release)
        {
            Common = common;
            Debug = debug;
            Development = development;
            Release = release;
        }
    }
    public sealed class LinkFlags
    {
        public readonly string[] Debug;
        public readonly string[] Development;
        public readonly string[] Release;

        public LinkFlags(string[] debug, string[] development, string[] release)
        {
            Debug = debug;
            Development = development;
            Release = release;
        }
    }

    public readonly PreprocessorDefines Preprocessor;
    public readonly CppFlags Cpp;
    public readonly LinkFlags Link;

    public CompilerFlags(PreprocessorDefines preprocessor, CppFlags cpp, LinkFlags link)
    {
        Preprocessor = preprocessor;
        Cpp = cpp;
        Link = link;
    }
}


// NOTE(v.matushkin): Naming is kinda meh
internal sealed class BuildConfigurationConfig
{
    public readonly CompilerFlags Msvc;

    public BuildConfigurationConfig(CompilerFlags msvc)
    {
        Msvc = msvc;
    }
}
