using System.IO;
using System.Runtime.CompilerServices;

using Hesternal.BuildTool;
using Hesternal.Core.Net7;


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

    public sealed class HesternalDirs
    {
        private readonly DirectoryItem m_hesternalDir;

        public readonly DirectoryItem ConfigDir;
        public readonly DirectoryItem SourceDir;

        public HesternalDirs(DirectoryItem rootDir)
        {
            m_hesternalDir = rootDir.MakeSubDirItem("Hesternal");

            ConfigDir = m_hesternalDir.MakeSubDirItem("Config");
            SourceDir = m_hesternalDir.MakeSubDirItem("Source");
        }
    }


    public static readonly DirectoryItem RootDir;
    public static readonly BuildDirs Build;
    public static readonly HesternalDirs Hesternal;


    static Globals()
    {
        RootDir = _GetRootDir();

        Build = new(RootDir);
        Hesternal = new(RootDir);

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
        return new HesternalSolution();
    }
}
