using Copium.BuildTool.BuildRules;

public sealed class Core : ModuleBuildRules
{
    public Core(TargetBuildRules target)
    {
        PrivateLibraries.Conan.Add(ConanLibrary.glfw);
        PublicLibraries.Conan.Add(ConanLibrary.simdjson);
        PublicLibraries.Conan.Add(ConanLibrary.spdlog);
    }
}
