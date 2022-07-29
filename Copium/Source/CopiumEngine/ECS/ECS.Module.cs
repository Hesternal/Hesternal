using Copium.BuildTool.BuildRules;

public sealed class ECS : ModuleBuildRules
{
    public ECS(TargetBuildRules target)
    {
        PublicLibraries.Conan.Add(ConanLibrary.entt);
    }
}
