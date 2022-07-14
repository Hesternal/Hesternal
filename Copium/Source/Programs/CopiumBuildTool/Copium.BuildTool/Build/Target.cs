using Copium.BuildTool.Core;

namespace Copium.BuildTool.Build;


internal abstract class Target
{
    protected readonly DirectoryItem m_targetDir;

    public abstract string TargetName { get; }
    public string TargetDirPath => m_targetDir.FullPath;


    protected Target(DirectoryItem targetDir)
    {
        m_targetDir = targetDir;
    }
}
