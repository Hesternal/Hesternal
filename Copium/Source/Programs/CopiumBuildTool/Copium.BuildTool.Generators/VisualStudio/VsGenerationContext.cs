using Copium.Core.Net7;

namespace Copium.BuildTool.Generators.VisualStudio;


internal sealed class VsGenerationContext
{
    private const string k_DirectoryBuildPropsFileName = "Directory.Build.props";
    private const string k_SubDirName = "vs";

    public readonly DirectoryItem SolutionDir;


    public VsGenerationContext(DirectoryItem rootDir, DirectoryItem solutionDir)
    {
        SolutionDir = solutionDir;

        _GeneratePropsFile_DirectoryBuild(rootDir, solutionDir);
    }


    public static (string project, string condition) GetConanLibPropsImportInfo(ConanLibrary library)
    {
        string conanPropsFileName = "conan_" + library.ToString();

        string conanImportProject = @"$(CopiumConanDir)\" + conanPropsFileName + VsFileExtension.Props;
        string conanImportCondition = $"'$({conanPropsFileName}_props_imported)' != 'True'";

        return (conanImportProject, conanImportCondition);
    }


    // TODO(v.matushkin): I think I should generate this from "makefile"
    private static void _GeneratePropsFile_DirectoryBuild(DirectoryItem rootDir, DirectoryItem solutionDir)
    {
        using VsXmlWriter propsFileWriter = new(solutionDir.MakeSubFileItem(k_DirectoryBuildPropsFileName));

        propsFileWriter.BeginProject(null, null, null, true);

        propsFileWriter.BeginPropertyGroup();
        //propsFileWriter.Property("CopiumRootDir", CopiumGlobal.RootDir.FullPath);
        propsFileWriter.Property("CopiumRootDir", rootDir.FullPath); // TODO: <DirectoryBuildProps> HACK
        //- Build dirs
        propsFileWriter.Property("CopiumBuildDir", @"$(CopiumRootDir)\build");
        propsFileWriter.Property("CopiumBinDir", @"$(CopiumBuildDir)\bin");
        propsFileWriter.Property("CopiumConanDir", @"$(CopiumBuildDir)\conan\" + k_SubDirName);
        propsFileWriter.Property("CopiumGeneratedDir", @"$(CopiumBuildDir)\generated");
        propsFileWriter.Property("CopiumIntermediateDir", @"$(CopiumBuildDir)\intermediate\" + k_SubDirName);
        //- Source dirs
        propsFileWriter.Property("CopiumConfigDir", @"$(CopiumRootDir)\Copium\Config");
        propsFileWriter.Property("CopiumSourceDir", @"$(CopiumRootDir)\Copium\Source");
        //- Target
        propsFileWriter.Property("CopiumTargetName", "$(MSBuildProjectName)");
        propsFileWriter.Property("CopiumTargetGeneratedDir", @"$(CopiumGeneratedDir)\$(CopiumTargetName)");
        propsFileWriter.Property("CopiumTargetSourceDir", @"$(CopiumSourceDir)\$(CopiumTargetName)");
        propsFileWriter.EndElement(); // BeginPropertyGroup

        propsFileWriter.EndElement(); // BeginProject
    }
}
