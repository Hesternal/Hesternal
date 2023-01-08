using Hesternal.BuildTool;
using Hesternal.CSharp.Net7;


internal sealed class HesternalEditor : HesternalCppProject
{
    public HesternalEditor(DirectoryItem outputDir)
        : base("HesternalEditor", outputDir)
    {
        BaseGeneratedIncludeDir = "EditorCht";

        SetConfigurationMatrix(new HesternalCppConfigurationMatrix(OutputType.Executable));

        AddPrivateProjectReference<HesternalEngine>();

        AddSourcesDir("Assets");
        AddSourcesDir("Editor");
        AddSourcesDir("GUI");
    }


    protected override void OnConfigureHesternalProject(HesternalCppConfiguration configuration)
    {
        //- Assets
        configuration.PrivateConanLibs.Add(ConanLibrary.assimp);
        configuration.PrivateConanLibs.Add(ConanLibrary.stb);
        //- Editor
        configuration.PrivateConanLibs.Add(ConanLibrary.cxxopts);
    }
}
