using Copium.BuildTool.BuildRules;

public sealed class CopiumEditor : TargetBuildRules
{
    public override TargetType eTargetType => TargetType.Editor;
    public override string BaseGeneratedIncludeDir => "EditorCht";

    public CopiumEditor(TargetBuildInfo targetBuildInfo)
        : base(targetBuildInfo)
    {
        TargetReference = "CopiumEngine";
    }
}
