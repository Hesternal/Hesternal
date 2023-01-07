namespace Hesternal.BuildTool;


public enum ConanLibrary
{
    assimp,
    catch2,
    cxxopts,
    entt,
    imgui,
    spdlog,
    stb,
}

public static class SystemLibs
{
    public static class Windows
    {
        public const string D3D11       = "d3d11.lib";
        public const string D3DCompiler = "d3dcompiler.lib";
        public const string DXGI        = "dxgi.lib";
        public const string DXGuid      = "dxguid.lib";
    }
}
