//using System.Xml;
//
////using Hesternal.BuildTool;
////using Hesternal.BuildTool.Build;
//using Hesternal.Core.Net7;
//
//namespace Hesternal.BuildTool.Generators.VisualStudio;
//
//
//internal abstract class VsProjectFile : ProjectFile
//{
//    public abstract string ProjectTypeGuid { get; }
//    public abstract string ProjectGuid { get; }
//
//
//    protected VsProjectFile(DirectoryItem projectDir, string projectName, string projectExtension)
//        : base(projectDir, projectName, projectExtension)
//    {
//    }
//
//    /// <summary>
//    /// For external project files
//    /// </summary>
//    protected VsProjectFile(DirectoryItem projectDir, FileItem projectFile, string projectName)
//        : base(projectDir, projectFile, projectName)
//    {
//    }
//}
//
//
//internal sealed class VsCsharpProjectFile : VsProjectFile
//{
//    /// <summary>
//    /// Sdk style .csproj doesn't actually contain ProjectGuid, it's defined in .sln file
//    /// </summary>
//    private readonly string m_projectGuid;
//
//    public override string ProjectTypeGuid => VisualStudioGuid.Csproj;
//    public override string ProjectGuid => m_projectGuid;
//
//
//    public VsCsharpProjectFile(DirectoryItem projectDir, string projectName)
//        : base(projectDir, projectName, VsFileExtension.Csproj)
//    {
//        m_projectGuid = VisualStudioGuid.Generate();
//    }
//
//    /// <summary>
//    /// For external project files
//    /// </summary>
//    public VsCsharpProjectFile(DirectoryItem projectDir, FileItem projectFile, string projectName, string projectGuid)
//        : base(projectDir, projectFile, projectName)
//    {
//        m_projectGuid = projectGuid;
//    }
//}
//
//
//internal sealed class VsCppProjectFile : VsProjectFile
//{
//    private readonly string m_projectGuid;
//
//    public readonly FileItem? PropsPrivateFile;
//    public readonly FileItem? PropsPublicFile;
//
//    public override string ProjectTypeGuid => VisualStudioGuid.Vcxproj;
//    public override string ProjectGuid => m_projectGuid;
//
//
//    public VsCppProjectFile(DirectoryItem projectDir, CppProject cppProject)
//        : base(projectDir, cppProject.Name, VsFileExtension.Vcxproj)
//    {
//        m_projectGuid = ProjectFileExist ? _GetGuidFromProjectFile() : VisualStudioGuid.Generate();
//
//        if (cppProject.eOutputType == OutputType.StaticLib)
//        {
//            PropsPrivateFile = ProjectDir.MakeSubFileItem(ProjectName + VsFileExtension.PropsPrivate);
//            PropsPublicFile = ProjectDir.MakeSubFileItem(ProjectName + VsFileExtension.PropsPublic);
//        }
//        else
//        {
//            PropsPrivateFile = null;
//            PropsPublicFile = null;
//        }
//    }
//
//
//    private string _GetGuidFromProjectFile()
//    {
//        XmlReaderSettings settings = new() { CloseInput = true };
//
//        string guid;
//
//        using (XmlReader projectFileReader = XmlReader.Create(OpenRead(), settings))
//        {
//            if (projectFileReader.ReadToFollowing("ProjectGuid") == false)
//            {
//                throw new CbtException($"Couldn't find ProjectGuid in the \"Globals\" PropertyGroup: {ProjectFileName}");
//            }
//
//            guid = projectFileReader.ReadElementContentAsString();
//        }
//
//        if (VisualStudioGuid.Validate(guid) == false)
//        {
//            // NOTE(v.matushkin): I can just generate new Guid a log a warning here
//            throw new CbtException($"\"{ProjectFileName}\" had invalid Guid");
//        }
//
//        return guid;
//    }
//}
