using Hesternal.CSharp.Net7;

namespace Hesternal.BuildTool.Generators.VisualStudio;


internal sealed class VsPropsFile
{
    public readonly FileItem File;

    public readonly string ImportConditionVariable; // NOTE(v.matushkin): Span? Will be useless anyway, cause XmlWriter takes only strings
    public readonly string ImportCondition;


    public VsPropsFile(FileItem propsFile, DirectoryItem propsFileDir)
    {
        File = propsFile;

        ImportConditionVariable = string.Concat(string.Join('_', propsFile.FileName.Split('.', System.StringSplitOptions.TrimEntries)), "_imported");
        ImportCondition = "'$(" + ImportConditionVariable + ")' != 'True'";
    }
}
