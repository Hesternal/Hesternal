using System.Collections.Generic;

namespace Copium.BuildTool.BuildRules;


public enum TargetType
{
    Engine,
    Editor,
}

public sealed class PreprocessorDefines
{
    public readonly List<string> Common = new();
    public readonly List<string> Debug = new();
    public readonly List<string> Development = new();
    public readonly List<string> Release = new();
}


public abstract class TargetBuildRules
{
    public static readonly string FileExtension = ".Target.cs";

    public readonly BuildOS OS;
    public readonly BuildCompiler Compiler;

    public readonly PreprocessorDefines PublicDefines = new();

    public string? TargetReference { get; protected set; }

    public abstract TargetType eTargetType { get; }
    public abstract string BaseGeneratedIncludeDir { get; }


    protected TargetBuildRules(TargetBuildInfo targetBuildInfo)
    {
        OS = targetBuildInfo.OS;
        Compiler = targetBuildInfo.Compiler;
    }
}
