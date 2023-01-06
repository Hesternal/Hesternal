using Copium.BuildTool.BuildRules;

public sealed class Platform : ModuleBuildRules
{
    public Platform(TargetBuildRules target)
    {
        if (target.OS != BuildOS.Windows)
        {
            Filter.PublicHeaderDirs.Add("Windows");
            Filter.SourceDirs.Add("Windows");
        }
    }
}
