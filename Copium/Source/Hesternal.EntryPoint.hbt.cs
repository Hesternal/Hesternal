using System.IO;
using System.Runtime.CompilerServices;

using Copium.BuildTool;
using Copium.Core.Net7;


internal static class Globals
{
    public sealed class BuildDirs
    {
        private readonly DirectoryItem m_buildDir;

        public readonly DirectoryItem BinDir;
        public readonly DirectoryItem ConanDir;
        public readonly DirectoryItem GeneratedDir;
        public readonly DirectoryItem IntermediateDir;
        public readonly DirectoryItem ProjectDir;

        public readonly DirectoryItem BinDotnetDir;

        public BuildDirs(DirectoryItem rootDir)
        {
            m_buildDir = rootDir.MakeSubDirItem("build");

            BinDir          = m_buildDir.MakeSubDirItem("bin");
            ConanDir        = m_buildDir.MakeSubDirItem("conan");
            GeneratedDir    = m_buildDir.MakeSubDirItem("generated");
            IntermediateDir = m_buildDir.MakeSubDirItem("intermediate");
            ProjectDir      = m_buildDir.MakeSubDirItem("project");

            BinDotnetDir = BinDir.MakeSubDirItem("dotnet");
        }
    }

    public sealed class CopiumDirs
    {
        private readonly DirectoryItem m_copiumDir;

        public readonly DirectoryItem ConfigDir;
        public readonly DirectoryItem SourceDir;

        public CopiumDirs(DirectoryItem rootDir)
        {
            m_copiumDir = rootDir.MakeSubDirItem("Copium");

            ConfigDir = m_copiumDir.MakeSubDirItem("Config");
            SourceDir = m_copiumDir.MakeSubDirItem("Source");
        }
    }


    public static readonly DirectoryItem RootDir;
    public static readonly BuildDirs Build;
    public static readonly CopiumDirs Copium;


    static Globals()
    {
        RootDir = _GetRootDir();

        Build = new(RootDir);
        Copium = new(RootDir);

        _CreateBuildDirs();
    }

    private static void _CreateBuildDirs()
    {
        Build.BinDir.Create();
        Build.ConanDir.Create();
        Build.GeneratedDir.Create();
        Build.IntermediateDir.Create();
        Build.ProjectDir.Create();

        Build.BinDotnetDir.Create();
    }

    private static DirectoryItem _GetRootDir([CallerFilePath] string solutionFilePath = "")
    {
        return new DirectoryItem(Path.Join(Path.GetDirectoryName(solutionFilePath), @"..\.."));
    }
}


public static class Main
{
    private static Solution EntryPoint()
    {
        return new CopiumSolution();
    }
}
