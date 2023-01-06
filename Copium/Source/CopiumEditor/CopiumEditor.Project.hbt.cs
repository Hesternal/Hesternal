using Copium.BuildTool;
using Copium.Core.Net7;


internal sealed class CopiumEditor : CopiumCppProject
{
    public CopiumEditor(DirectoryItem outputDir)
        : base("CopiumEditor", outputDir)
    {
        BaseGeneratedIncludeDir = "EditorCht";

        SetConfigurationMatrix(new CopiumCppConfigurationMatrix(OutputType.Executable));

        AddPrivateProjectReference<CopiumEngine>();

        AddSourcesDir("Assets");
        AddSourcesDir("Editor");
        AddSourcesDir("GUI");
    }


    protected override void OnConfigureCopiumProject(CopiumCppConfiguration configuration)
    {
        //- Assets
        configuration.PrivateConanLibs.Add(ConanLibrary.assimp);
        configuration.PrivateConanLibs.Add(ConanLibrary.stb);
        //- Editor
        configuration.PrivateConanLibs.Add(ConanLibrary.cxxopts);
    }
}
