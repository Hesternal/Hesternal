using System;
using System.Diagnostics.CodeAnalysis;
using System.IO;

using Copium.BuildTool.Platform;

using Microsoft.Extensions.Logging;

namespace Copium.BuildTool.Core;


// NOTE(v.matushkin): Naming ?
internal static class CopiumGlobal
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


    private const string k_CopiumRootDirEnvVariable = "COPIUM_ROOT_DIR";

    public readonly static DirectoryItem RootDir;
    public readonly static BuildDirs Build;
    public readonly static CopiumDirs Copium;

    public readonly static BuildConfigurationConfig BuildConfigurations;


    static CopiumGlobal()
    {
        RootDir = _FindRootDir();

        Build = new(RootDir);
        Copium = new(RootDir);

        BuildConfigurations = Copium.ConfigDir.MakeSubFileItem("BuildConfiguration.json").JsonDeserialize<BuildConfigurationConfig>();
    }


    public static void CreateBuildDirs()
    {
        Build.BinDir.Create();
        Build.ConanDir.Create();
        Build.GeneratedDir.Create();
        Build.IntermediateDir.Create();
        Build.ProjectDir.Create();

        Build.BinDotnetDir.Create();
    }


    private static DirectoryItem _FindRootDir()
    {
        string? rootDirPath;

        if (_TryGetRootDirFromEnvironment(out rootDirPath))
        {
            DirectoryItem rootDir = new(rootDirPath);
            if (_ValidateRootDir(rootDir))
            {
                return rootDir;
            }
            else
            {
                Log.Logger.LogWarning(
                    "Environment variable {variableName} were specified but not valid [{variableValue}]",
                    k_CopiumRootDirEnvVariable, rootDirPath);
            }
        }

        if (_TryGetRootDirFromAppLocation(out rootDirPath))
        {
            DirectoryItem rootDir = new(rootDirPath);
            if (_ValidateRootDir(rootDir))
            {
                return rootDir;
            }
        }

        throw new CbtException("Couldn't determine root dir");
    }

    private static bool _TryGetRootDirFromEnvironment([NotNullWhen(true)] out string? rootDirPath)
    {
        rootDirPath = Environment.GetEnvironmentVariable(k_CopiumRootDirEnvVariable);

        if (string.IsNullOrEmpty(rootDirPath))
        {
            return false;
        }

        return true;
    }

    private static bool _TryGetRootDirFromAppLocation([NotNullWhen(true)] out string? rootDirPath)
    {
        string appDir = AppDomain.CurrentDomain.BaseDirectory;

        // Potential app locations
        // ROOT_DIR\build\bin\dotnet\CopiumBuildTool.exe
        // ROOT_DIR\build\intermediate\dotnet\Copium.BuildTool\bin\CONFIGURATION\CopiumBuildTool.exe
        string threeLevelsUp = Path.Join("..", "..", "..");
        string rootDirlevelsUp = Path.GetFileName(Path.GetDirectoryName(appDir)) switch
        {
            "dotnet" => threeLevelsUp,
            "Debug" or "Release" => Path.Join(threeLevelsUp, threeLevelsUp),
            _ => throw new CbtException("Couldn't determine CopiumBuildTool location")
        };

        rootDirPath = Path.GetFullPath(rootDirlevelsUp, appDir);

        if (string.IsNullOrEmpty(rootDirPath))
        {
            return false;
        }

        return true;
    }

    private static bool _ValidateRootDir(DirectoryItem potentialRootDir)
    {
        if (potentialRootDir.Exists == false)
        {
            return false;
        }

        FileItem gitignoreFile = potentialRootDir.MakeSubFileItem(".gitignore");
        if (gitignoreFile.Exists == false)
        {
            return false;
        }

        FileItem conanfileFile = potentialRootDir.MakeSubFileItem("conanfile.py");
        if (conanfileFile.Exists == false)
        {
            return false;
        }

        return true;
    }
}
