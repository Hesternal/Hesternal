using System.IO;
using System.Text;

namespace Hesternal.BuildTool.SourceGenerator;


internal static class MemoryStreamExtensions
{
    // NOTE(v.matushkin): I have a doubt that this MemoryString->String conversion is very effective
    public static string ToUtf8String(this MemoryStream stream) => Encoding.UTF8.GetString(stream.ToArray());
}
