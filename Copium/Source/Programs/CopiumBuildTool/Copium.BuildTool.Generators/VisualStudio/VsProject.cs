using System;
using System.Collections.Generic;

using Copium.Core.Net7;

namespace Copium.BuildTool.Generators.VisualStudio;


internal abstract class VsProject<T> : IEquatable<VsProject<T>?>, IComparable<VsProject<T>?>
    where T : Project
{
    public readonly T Base;
    public readonly string ProjectGuid;
    public readonly FileItem OutputFile;

    public abstract string ProjectTypeGuid { get; }


    protected VsProject(T project, string projectFileExtension)
    {
        Base = project;
        ProjectGuid = VisualStudioGuid.Generate();
        OutputFile = project.OutputDir.MakeSubFileItem(project.Name + projectFileExtension);
    }


    public override bool Equals(object? obj) => Equals(obj as VsProject<T>);
    public bool Equals(VsProject<T>? other) => other is not null && EqualityComparer<T>.Default.Equals(Base, other.Base);
    public override int GetHashCode() => HashCode.Combine(Base);

    int IComparable<VsProject<T>?>.CompareTo(VsProject<T>? other)
    {
        if (other is null) return 1;
        return Comparer<T>.Default.Compare(Base, other.Base);
    }

    public static bool operator ==(VsProject<T>? left, VsProject<T>? right)
    {
        return EqualityComparer<VsProject<T>>.Default.Equals(left, right);
    }

    public static bool operator !=(VsProject<T>? left, VsProject<T>? right)
    {
        return !(left == right);
    }
}
