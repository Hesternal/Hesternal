using Copium.BuildTool.BuildRules;

public sealed class Graphics : ModuleBuildRules
{
    public Graphics(TargetBuildRules target)
    {
        if (target.OS == BuildOS.Windows)
        {
            PrivateLibraries.System.Add(LibraryDependencies.WindowsLibs.D3D11);
            PrivateLibraries.System.Add(LibraryDependencies.WindowsLibs.DXGI);
            PrivateLibraries.System.Add(LibraryDependencies.WindowsLibs.D3DCompiler);
            // TODO(v.matushkin): This should be added for Debug configuration only ?
            PrivateLibraries.System.Add(LibraryDependencies.WindowsLibs.DXGuid);
        }
        else
        {
            Filter.SourceDirs.Add("DirectX11");
        }
    }
}
