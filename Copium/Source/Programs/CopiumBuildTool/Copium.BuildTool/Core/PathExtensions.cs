using System.IO;

namespace Copium.BuildTool.Core;


internal static class PathExtensions
{
    public static string? GetParentDir(string dir)
    {
        int slashIndex = dir.LastIndexOf(Path.DirectorySeparatorChar);
        return slashIndex != -1 ? dir[..slashIndex] : null;
    }
}
