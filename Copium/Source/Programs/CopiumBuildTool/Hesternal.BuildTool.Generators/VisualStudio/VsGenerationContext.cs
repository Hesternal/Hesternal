using Hesternal.Core.Net7;

namespace Hesternal.BuildTool.Generators.VisualStudio;


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

        string conanImportProject = @"$(HesternalConanDir)\" + conanPropsFileName + VsFileExtension.Props;
        string conanImportCondition = $"'$({conanPropsFileName}_props_imported)' != 'True'";

        return (conanImportProject, conanImportCondition);
    }


    // TODO(v.matushkin): I think I should generate this from "makefile"
    private static void _GeneratePropsFile_DirectoryBuild(DirectoryItem rootDir, DirectoryItem solutionDir)
    {
        using VsXmlWriter propsFileWriter = new(solutionDir.MakeSubFileItem(k_DirectoryBuildPropsFileName));

        propsFileWriter.BeginProject(null, null, null, true);

        propsFileWriter.BeginPropertyGroup();
        //propsFileWriter.Property("HesternalRootDir", HesternalGlobal.RootDir.FullPath);
        propsFileWriter.Property("HesternalRootDir", rootDir.FullPath); // TODO: <DirectoryBuildProps> HACK
        //- Build dirs
        propsFileWriter.Property("HesternalBuildDir", @"$(HesternalRootDir)\build");
        propsFileWriter.Property("HesternalBinDir", @"$(HesternalBuildDir)\bin");
        propsFileWriter.Property("HesternalConanDir", @"$(HesternalBuildDir)\conan\" + k_SubDirName);
        propsFileWriter.Property("HesternalGeneratedDir", @"$(HesternalBuildDir)\generated");
        propsFileWriter.Property("HesternalIntermediateDir", @"$(HesternalBuildDir)\intermediate\" + k_SubDirName);
        //- Source dirs
        propsFileWriter.Property("HesternalConfigDir", @"$(HesternalRootDir)\Copium\Config");
        propsFileWriter.Property("HesternalSourceDir", @"$(HesternalRootDir)\Copium\Source");
        //- Target
        propsFileWriter.Property("HesternalTargetName", "$(MSBuildProjectName)");
        propsFileWriter.Property("HesternalTargetGeneratedDir", @"$(HesternalGeneratedDir)\$(HesternalTargetName)");
        propsFileWriter.Property("HesternalTargetSourceDir", @"$(HesternalSourceDir)\$(HesternalTargetName)");
        propsFileWriter.EndElement(); // BeginPropertyGroup

        propsFileWriter.EndElement(); // BeginProject
    }
}
