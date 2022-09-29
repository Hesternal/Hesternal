using Copium.Core.Net7;

namespace Copium.BuildTool;


public abstract class Project
{
    public readonly string Name;
    public readonly DirectoryItem OutputDir;

    /// <summary>
    /// .cbt.cs file
    /// </summary>
    public readonly FileItem MakeFile;
    /// <summary>
    /// .cbt.cs file directory
    /// </summary>
    public readonly DirectoryItem MakeFileDir;


    protected Project(string projectName, string projectFilePath, DirectoryItem outputDir)
    {
        Name = projectName;
        OutputDir = outputDir;

        MakeFile = new(projectFilePath);
        MakeFileDir = MakeFile.Directory;
    }
}
