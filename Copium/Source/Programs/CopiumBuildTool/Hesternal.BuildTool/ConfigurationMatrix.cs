using System.Collections.ObjectModel;

namespace Copium.BuildTool;


public enum BuildOS
{
    Linux,
    Windows,
}

public enum BuildCompiler
{
    Clang,
    Msvc,
}

public enum BuildPlatform
{
    x64,
    AnyCPU,
}

public enum OutputType
{
    Executable,
    SharedLib,
    StaticLib,
}


public abstract class ProjectConfigurationDesc
{
    public required BuildOS OS { get; init; }
    public required BuildPlatform Platform { get; init; }

    public abstract string Name { get; }
    public abstract OutputType OutputType { get; }
}


public abstract class CppConfigurationDesc : ProjectConfigurationDesc
{
    public required BuildCompiler Compiler { get; init; }
}


public interface ICppConfigurationMatrix
{
    public ReadOnlyCollection<CppProjectConfiguration> MakeConfigurations();
}
