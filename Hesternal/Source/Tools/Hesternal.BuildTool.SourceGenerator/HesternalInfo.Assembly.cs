namespace Hesternal.BuildTool.SourceGenerator;


internal static partial class HesternalInfo
{
    public static class Assembly
    {
        // TODO(v.matushkin): <Hardcoded/ProjectName>
        private const string k_BaseName = "Hesternal";

        public const string BuildTool = k_BaseName + ".BuildTool";
        public const string BuildToolGenerators = BuildTool + ".Generators";
    }
}
