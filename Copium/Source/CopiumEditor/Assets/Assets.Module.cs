using Copium.BuildTool.BuildRules;

public sealed class Assets : ModuleBuildRules
{
    public Assets(TargetBuildRules targetBuildRules)
    {
        PrivateLibraries.Conan.Add(ConanLibrary.stb);
    }
}
