using System.Collections.Generic;

namespace Copium.BuildTool.BuildRules;


public enum ConanLibrary
{
    assimp,
    cxxopts,
    entt,
    glfw,
    simdjson,
    spdlog,
    stb,
}

public sealed class LibraryDependencies
{
    public static class WindowsLibs
    {
        public const string D3D11       = "d3d11.lib";
        public const string D3DCompiler = "d3dcompiler.lib";
        public const string DXGI        = "dxgi.lib";
        public const string DXGuid      = "dxguid.lib";
    }

    public readonly List<ConanLibrary> Conan = new();
    public readonly List<string> System = new();
}

public sealed class FileFilter
{
    public readonly HashSet<string> PublicHeaderDirs = new();
    public readonly HashSet<string> SourceDirs = new();
}


public abstract class ModuleBuildRules
{
    public static readonly string FileExtension = ".Module.cs";

    public readonly LibraryDependencies PrivateLibraries = new();
    public readonly LibraryDependencies PublicLibraries = new();

    public readonly FileFilter Filter = new();
}
