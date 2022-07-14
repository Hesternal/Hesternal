using System.Collections.Generic;
using System.Linq;

using Copium.BuildTool.BuildRules;
using Copium.BuildTool.Core;

namespace Copium.BuildTool.Build;


internal sealed class CppTarget : Target
{
    private readonly TargetBuildRules m_targetBuildRules;

    public readonly Module[] Modules;

    public override string TargetName => m_targetDir.Name;

    public TargetType eTargetType => m_targetBuildRules.eTargetType;
    public string BaseGeneratedIncludeDir => m_targetBuildRules.BaseGeneratedIncludeDir;
    public string? TargetReference => m_targetBuildRules.TargetReference;

    public PreprocessorDefines PublicDefines => m_targetBuildRules.PublicDefines;

    public IEnumerable<string> IncludeDirs => Modules.SelectMany(module => module.EnumerateIncludeDirs());

    public IEnumerable<string> PublicIncludeDirs => Modules.Where(module => module.PublicIncludeDir != null)
            .Select(dir => dir.PublicIncludeDir!.FullPath);

    public IEnumerable<string> PublicModuleDirs => Modules.Where(module => module.PublicModuleDirs != null)
            .SelectMany(dirs => dirs.PublicModuleDirs!)
            .Select(dir => dir.FullPath);

    public IEnumerable<ConanLibrary> PrivateConanLibs => Modules.SelectMany(module => module.PrivateLibraries.Conan);
    public IEnumerable<ConanLibrary> PublicConanLibs => Modules.SelectMany(module => module.PublicLibraries.Conan);

    public IEnumerable<string> PrivateSystemLibs => Modules.SelectMany(module => module.PrivateLibraries.System);


    public CppTarget(TargetBuildInfo targetBuildInfo, DirectoryItem targetDir, BuildRulesAssembly rulesAssembly)
        : base(targetDir)
    {
        m_targetBuildRules = rulesAssembly.CreateTargetBuildRules(targetDir.Name, targetBuildInfo);
        Modules = _GetModules(rulesAssembly, targetDir, m_targetBuildRules);
    }


    private static Module[] _GetModules(BuildRulesAssembly rulesAssembly, DirectoryItem targetDir, TargetBuildRules targetBuildRules)
    {
        List<Module> modules = new();

        foreach (DirectoryItem moduleDir in targetDir.EnumerateDirectories())
        {
            ModuleBuildRules moduleBuildRules = rulesAssembly.CreateModuleBuildRules(moduleDir.Name, targetBuildRules);
            modules.Add(new Module(moduleDir, moduleBuildRules));
        }

        return modules.ToArray();
    }
}
