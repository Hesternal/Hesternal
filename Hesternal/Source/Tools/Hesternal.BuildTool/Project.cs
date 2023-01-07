using System;
using System.Collections.Generic;

using Hesternal.Core.Net7;

namespace Hesternal.BuildTool;


public abstract class Project : IEquatable<Project?>, IComparable<Project?>
{
    public readonly string Name;
    public readonly DirectoryItem OutputDir;

    /// <summary>
    /// .hbt.cs file
    /// </summary>
    public readonly FileItem MakeFile;
    /// <summary>
    /// .hbt.cs file directory
    /// </summary>
    public readonly DirectoryItem MakeFileDir;


    protected Project(string projectName, string projectFilePath, DirectoryItem outputDir)
    {
        Name = projectName;
        OutputDir = outputDir;

        MakeFile = new(projectFilePath);
        MakeFileDir = MakeFile.Directory;
    }


    public override bool Equals(object? obj) => Equals(obj as Project);
    public bool Equals(Project? other) => other is not null && Name == other.Name;
    public override int GetHashCode() => HashCode.Combine(Name);

    int IComparable<Project?>.CompareTo(Project? other)
    {
        if (other is null) return 1;
        return Name.CompareTo(other.Name);
    }

    public static bool operator ==(Project? left, Project? right)
    {
        return EqualityComparer<Project>.Default.Equals(left, right);
    }

    public static bool operator !=(Project? left, Project? right)
    {
        return !(left == right);
    }
}
