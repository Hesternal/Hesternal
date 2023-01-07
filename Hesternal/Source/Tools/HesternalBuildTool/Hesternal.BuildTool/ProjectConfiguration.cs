namespace Hesternal.BuildTool;


public abstract class ProjectConfiguration
{
    public readonly BuildOS OS;
    public readonly BuildPlatform Platform;
    public readonly OutputType OutputType;
    public readonly string Name;

    public readonly string PlatformString;


    protected ProjectConfiguration(ProjectConfigurationDesc desc)
    {
        OS = desc.OS;
        Platform = desc.Platform;
        OutputType = desc.OutputType;
        Name = desc.Name;

        PlatformString = desc.Platform.ToString();
    }
}


//public class CSharpProjectConfiguration : ProjectConfiguration
//{
//    public CSharpProjectConfiguration(BuildOS os, BuildPlatform platform, BuildConfiguration configuration)
//        : base(os, platform, configuration, _MakeCsharpConfigurationName(configuration))
//    {
//    }
//
//
//    private static string _MakeCsharpConfigurationName(BuildConfiguration configuration)
//    {
//        return configuration.ToString();
//    }
//}
