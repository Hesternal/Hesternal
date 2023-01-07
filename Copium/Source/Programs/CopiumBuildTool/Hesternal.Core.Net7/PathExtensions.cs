using System.IO;

namespace Hesternal.Core.Net7;


public static class PathExtensions
{
    public static string? GetParentDir(string dir)
    {
        int slashIndex = dir.LastIndexOf(Path.DirectorySeparatorChar);
        return slashIndex != -1 ? dir[..slashIndex] : null;
    }
}
