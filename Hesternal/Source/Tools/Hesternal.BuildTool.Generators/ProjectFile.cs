using System.IO;

using Hesternal.CSharp.Net7;

namespace Hesternal.BuildTool.Generators;


internal abstract class ProjectFile
{
    private readonly FileItem m_projectFile;

    public readonly DirectoryItem ProjectDir;
    public readonly string ProjectName;

    public string ProjectFileName  => m_projectFile.FileName;
    public string ProjectFilePath  => m_projectFile.FullPath;
    public bool   ProjectFileExist => m_projectFile.Exists;


    protected ProjectFile(DirectoryItem projectDir, string projectName, string projectExtension)
    {
        m_projectFile = projectDir.MakeSubFileItem(projectName + projectExtension);
        ProjectDir = projectDir;
        ProjectName = projectName;
    }

    /// <summary>
    /// For external project files
    /// </summary>
    protected ProjectFile(DirectoryItem projectDir, FileItem projectFile, string projectName)
    {
        m_projectFile = projectFile;
        ProjectDir = projectDir;
        ProjectName = projectName;
    }


    public string GetRelativePath(DirectoryItem relativeTo) => m_projectFile.GetRelativePath(relativeTo);

    public StreamReader OpenRead() => m_projectFile.OpenReadText();
    public StreamWriter OpenWrite() => m_projectFile.OpenWriteText();
}
