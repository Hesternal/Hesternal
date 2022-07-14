using Copium.BuildTool.BuildRules;

public sealed class Editor : ModuleBuildRules
{
    public Editor(TargetBuildRules targetBuildRules)
    {
        PrivateLibraries.Conan.Add(ConanLibrary.cxxopts);
    }
}
