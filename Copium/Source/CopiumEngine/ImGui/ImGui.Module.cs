using Copium.BuildTool.BuildRules;

public sealed class ImGui : ModuleBuildRules
{
    public ImGui(TargetBuildRules target)
    {
        PublicLibraries.Conan.Add(ConanLibrary.imgui);
    }
}
