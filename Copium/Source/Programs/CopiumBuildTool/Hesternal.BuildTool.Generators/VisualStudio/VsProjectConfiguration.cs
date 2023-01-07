namespace Copium.BuildTool.Generators.VisualStudio;


internal abstract class VsProjectConfiguration<T>
    where T : ProjectConfiguration
{
    public readonly T Base;
    /// <summary>
    /// "Configuration|Platform" string, like "Debug|x64"
    /// </summary>
    public readonly string ConfigurationPlatform;
    /// <summary>
    /// "'$(Configuration)|$(Platform)'=='Debug|x64'" string
    /// </summary>
    public readonly string Condition;


    protected VsProjectConfiguration(T configuration)
    {
        Base = configuration;
        ConfigurationPlatform = configuration.Name + "|" + configuration.PlatformString;
        Condition = $"'$(Configuration)|$(Platform)'=='{ConfigurationPlatform}'";
    }
}


//internal sealed class VsCsharpProjectConfiguration : VsProjectConfiguration<CSharpProjectConfiguration>
//{
//    public VsCsharpProjectConfiguration(CSharpProjectConfiguration configuration)
//        : base(configuration)
//    {
//    }
//}


internal sealed class VsCppProjectConfiguration : VsProjectConfiguration<CppProjectConfiguration>
{
    public VsCppProjectConfiguration(CppProjectConfiguration configuration)
        : base(configuration)
    {
    }
}
