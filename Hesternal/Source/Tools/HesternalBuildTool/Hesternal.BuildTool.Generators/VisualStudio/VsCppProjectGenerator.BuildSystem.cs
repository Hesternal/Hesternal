namespace Hesternal.BuildTool.Generators.VisualStudio;


internal sealed partial class VsCppProjectGenerator
{
    private void _BuildSystemConfigurationPropertyGroup(MSBuildOptions options)
    {
        m_projectWriter.Property(nameof(options.PlatformToolset), options.PlatformToolset.ToString());
        if (options.PreferredToolArchitecture != MSBuild.PreferredToolArchitecture.Default)
        {
            m_projectWriter.Property(nameof(options.PreferredToolArchitecture), options.PreferredToolArchitecture.ToString());
        }
        m_projectWriter.Property(nameof(options.UseDebugLibraries), options.UseDebugLibraries);
        m_projectWriter.Property(nameof(options.CharacterSet), options.CharacterSet.ToString());
    }

    private void _BuildSystemPerConfigurationPropertyGroup(MSBuildOptions options)
    {
        m_projectWriter.Property(nameof(options.CopyLocalDeploymentContent), options.CopyLocalDeploymentContent);
        m_projectWriter.Property(nameof(options.CopyLocalProjectReference), options.CopyLocalProjectReference);
        m_projectWriter.Property(nameof(options.CopyLocalDebugSymbols), options.CopyLocalDebugSymbols);
        m_projectWriter.Property(nameof(options.CopyCppRuntimeToOutputDir), options.CopyCppRuntimeToOutputDir);
    }
}
