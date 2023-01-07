using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.CompilerServices;

using Hesternal.Core.Net7;

namespace Hesternal.BuildTool;


public abstract class CppProject : Project
{
    /// <summary>
    /// If null -> do not run HesternalHeaderTool for this project
    /// </summary>
    public string? BaseGeneratedIncludeDir { get; protected set; }

    public ReadOnlyCollection<CppProject>? PrivateProjectReferences { get; private set; }
    public ReadOnlyCollection<CppProject>? PublicProjectReferences { get; private set; }

    public ReadOnlyCollection<CppProjectConfiguration> Configurations => m_configurations ?? throw new NullReferenceException("Forgot to configure project?");

    public readonly ReadOnlyCollection<string> PrivateIncludeDirs;
    public readonly ReadOnlyCollection<string> PublicIncludeDirs;
    public IEnumerable<string> IncludeDirs => m_privateIncludeDirs.Concat(m_publicIncludeDirs);

    /// <remark>
    /// For some reason, I need to list every subdir of public module dir in vcxproj PublicModuleDirectories property
    ///  to make it work for the targets consuming libraries with this property.
    /// May be I'm doing something wrong, may be it's a bug, may be this is how it's supposed to be done, idk.
    ///
    /// It does work for cases like this though:
    ///  HesternalEditor
    ///    Some .cpp file:
    ///      import File.ixx
    ///      import FileBinaryConverter.ixx
    ///  HesternalEngine
    ///    PublicModuleDirectories <- vcxproj property
    ///      Public/Core
    ///      Public/Core/Serialization
    ///    Public/                 <- public module dir structure
    ///      Core/
    ///        Platform/
    ///          File.ixx
    ///        Serialization/
    ///          FileBinaryConverter.ixx
    ///            import File.ixx
    ///
    /// Even though I'm not listing Public/Core/Platform dir, 'import File.ixx' works in the HesternalEditor
    ///  because FileBinaryConverter.ixx references it in the HesternalEngine
    /// </remark>
    public readonly ReadOnlyCollection<string> PublicModuleDirs;

    /// <summary>
    /// Private .hpp files
    /// </summary>
    public readonly ReadOnlyCollection<FileItem> PrivateHeaders;
    /// <summary>
    /// Public .hpp files
    /// </summary>
    public readonly ReadOnlyCollection<FileItem> PublicHeaders;
    /// <summary>
    /// Private .ixx files
    /// </summary>
    public readonly ReadOnlyCollection<FileItem> PrivateModules;
    /// <summary>
    /// Public .ixx files
    /// </summary>
    public readonly ReadOnlyCollection<FileItem> PublicModules;
    /// <summary>
    /// .cpp files
    /// </summary>
    public readonly ReadOnlyCollection<FileItem> Sources;


    private ICppConfigurationMatrix? m_configurationMatrix;
    private ReadOnlyCollection<CppProjectConfiguration>? m_configurations;

    private readonly List<Type> m_privateProjectReferences = new();
    private readonly List<Type> m_publicProjectReferences = new();

    private readonly List<string> m_privateIncludeDirs = new();
    private readonly List<string> m_publicIncludeDirs = new();
    private readonly List<string> m_publicModuleDirs = new();

    private readonly List<FileItem> m_privateHeaders = new();
    private readonly List<FileItem> m_publicHeaders = new();
    private readonly List<FileItem> m_privateModules = new();
    private readonly List<FileItem> m_publicModules = new();
    private readonly List<FileItem> m_sources = new();


    protected CppProject(string projectName, DirectoryItem outputDir, [CallerFilePath] string projectFilePath = "")
        : base(projectName, projectFilePath, outputDir)
    {
        PrivateIncludeDirs = m_privateIncludeDirs.AsReadOnly();
        PublicIncludeDirs = m_publicIncludeDirs.AsReadOnly();

        PublicModuleDirs = m_publicModuleDirs.AsReadOnly();

        PrivateHeaders = m_privateHeaders.AsReadOnly();
        PublicHeaders = m_publicHeaders.AsReadOnly();
        PrivateModules = m_privateModules.AsReadOnly();
        PublicModules = m_publicModules.AsReadOnly();
        Sources = m_sources.AsReadOnly();
    }


    public void Resolve(Solution solution)
    {
        PrivateProjectReferences = _ResolveProjectReferences(m_privateProjectReferences, solution);
        PublicProjectReferences = _ResolveProjectReferences(m_publicProjectReferences, solution);
    }

    public void ConfigureProject()
    {
        if (m_configurationMatrix == null)
        {
            throw new NullReferenceException("Forgot to set configuration matrix?");
        }

        m_configurations = m_configurationMatrix.MakeConfigurations();

        foreach (CppProjectConfiguration configuration in m_configurations)
        {
            OnConfigureProject(configuration);
        }
    }


    protected abstract void OnConfigureProject(CppProjectConfiguration configuration);


    protected void SetConfigurationMatrix(ICppConfigurationMatrix matrix) => m_configurationMatrix = matrix;

    protected void AddPrivateProjectReference<TProject>()
        where TProject : CppProject
    {
        m_privateProjectReferences.Add(typeof(TProject));
    }

    protected void AddPublicProjectReference<TProject>()
        where TProject : CppProject
    {
        m_publicProjectReferences.Add(typeof(TProject));
    }

    protected void AddPrivateIncludeDir(string privateIncludeDir) => m_privateIncludeDirs.Add(privateIncludeDir);
    protected void AddPublicIncludeDir(string publicIncludeDir) => m_publicIncludeDirs.Add(publicIncludeDir);

    protected void AddPublicModuleDirs(IEnumerable<string> publicModuleDirs) => m_publicModuleDirs.AddRange(publicModuleDirs);

    protected void AddPrivateHeaders(IEnumerable<FileItem> privateHeaders) => m_privateHeaders.AddRange(privateHeaders);
    protected void AddPublicHeaders(IEnumerable<FileItem> publicHeaders) => m_publicHeaders.AddRange(publicHeaders);
    protected void AddPrivateModules(IEnumerable<FileItem> privateModules) => m_privateModules.AddRange(privateModules);
    protected void AddPublicModules(IEnumerable<FileItem> publicModules) => m_publicModules.AddRange(publicModules);
    protected void AddSources(IEnumerable<FileItem> sources) => m_sources.AddRange(sources);


    private static ReadOnlyCollection<CppProject>? _ResolveProjectReferences(List<Type> references, Solution solution)
    {
        int referenceCount = references.Count;

        if (referenceCount == 0)
        {
            return null;
        }

        CppProject[] projectReferences = new CppProject[referenceCount];

        for (int i = 0; i < referenceCount; i++)
        {
            projectReferences[i] = solution.TypeToProject(references[i]);
        }

        return projectReferences.AsReadOnly();
    }
}
