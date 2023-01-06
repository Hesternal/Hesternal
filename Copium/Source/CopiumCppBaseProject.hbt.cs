using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

using Copium.BuildTool;
using Copium.Core.Net7;


internal abstract class CopiumCppBaseProject : CppProject
{
    protected static class SearchPattern
    {
        public const string Headers = "*.hpp";
        public const string Modules = "*.ixx";
        public const string Sources = "*.cpp";
    }


    protected CopiumCppBaseProject(string projectName, DirectoryItem outputDir, [CallerFilePath] string projectFilePath = "")
        : base(projectName, outputDir, projectFilePath)
    {
    }


    protected sealed override void OnConfigureProject(CppProjectConfiguration configuration)
    {
        CopiumCppConfiguration copiumConfiguration = (CopiumCppConfiguration)configuration;

        if (copiumConfiguration.BuildSystemOptions is MSBuildOptions msbuild)
        {
            copiumConfiguration.OutputDir = @"$(CopiumBinDir)\$(Configuration)\";
            copiumConfiguration.IntermediateDir = @"$(CopiumIntermediateDir)\$(Configuration)\$(ProjectName)\";
             _ConfigureBuildSystem(copiumConfiguration, msbuild);
        }
        else
        {
            throw new NotSupportedException("Only MSBuild build system supported for now");
        }

        if (copiumConfiguration.CompilerOptions is MsvcOptions msvc)
        {
            _ConfigureCompiler(copiumConfiguration, msvc);
        }
        else
        {
            throw new NotSupportedException("Only MSVC compiler supported for now");
        }

        if (copiumConfiguration.LinkerOptions is MsvcLibrarianOptions msvcLibrarian)
        {
            _ConfigureLinker(copiumConfiguration, msvcLibrarian);
        }
        else if (copiumConfiguration.LinkerOptions is MsvcLinkerOptions msvcLinker)
        {
            _ConfigureLinker(copiumConfiguration, msvcLinker);
        }
        else
        {
            throw new NotSupportedException("Only MSVC linker supported for now");
        }

        OnConfigureCopiumProject(copiumConfiguration);
    }


    protected virtual void OnConfigureCopiumProject(CopiumCppConfiguration configuration)
    {
    }


    private static void _ConfigureBuildSystem(CopiumCppConfiguration configuration, MSBuildOptions msbuild)
    {
        msbuild.PreferredToolArchitecture = MSBuild.PreferredToolArchitecture.x64;
        msbuild.CharacterSet = MSBuild.CharacterSet.Unicode;

        if (configuration.Configuration == BuildConfiguration.Debug)
        {
            msbuild.UseDebugLibraries = true;
        }
    }

    private static void _ConfigureCompiler(CopiumCppConfiguration configuration, MsvcOptions msvc)
    {
        msvc.ScanSourceForModuleDependencies = true;
        msvc.SuppressStartupBanner = true;
        msvc.WarningLevel = Msvc.WarningLevel.Level4;
        msvc.TreatWarningAsError = true;
        msvc.DiagnosticsFormat = Msvc.DiagnosticsFormat.Caret;
        msvc.MultiProcessorCompilation = true;
        msvc.UseStandardPreprocessor = true;
        msvc.BufferSecurityCheck = Msvc.BufferSecurityCheck.False;
        msvc.LanguageStandard = Msvc.LanguageStandard.stdcpplatest;
        msvc.ConformanceMode = Msvc.ConformanceMode.True;

        switch (configuration.Configuration)
        {
            case BuildConfiguration.Debug:
                msvc.DebugInformationFormat = Msvc.DebugInformationFormat.ProgramDatabase;
                msvc.Optimization = Msvc.Optimization.Disabled;
                msvc.InlineFunctionExpansion = Msvc.InlineFunctionExpansion.Disabled;
                msvc.WholeProgramOptimization = false;
                msvc.BasicRuntimeChecks = Msvc.BasicRuntimeChecks.EnableFastChecks;
                msvc.FunctionLevelLinking = false;
                msvc.RuntimeLibrary = Msvc.RuntimeLibrary.MultiThreadedDebugDLL;
                break;
            case BuildConfiguration.Development:
                msvc.DebugInformationFormat = Msvc.DebugInformationFormat.ProgramDatabase;
                msvc.Optimization = Msvc.Optimization.MaxSpeed;
                msvc.InlineFunctionExpansion = Msvc.InlineFunctionExpansion.OnlyExplicitInline;
                msvc.WholeProgramOptimization = false;
                msvc.IntrinsicFunctions = true;
                msvc.FavorSizeOrSpeed = Msvc.FavorSizeOrSpeed.Speed;
                msvc.FunctionLevelLinking = true;
                msvc.RuntimeLibrary = Msvc.RuntimeLibrary.MultiThreadedDLL;
                break;
            case BuildConfiguration.Release:
                msvc.DebugInformationFormat = Msvc.DebugInformationFormat.None;
                msvc.Optimization = Msvc.Optimization.MaxSpeed;
                msvc.InlineFunctionExpansion = Msvc.InlineFunctionExpansion.AnySuitable;
                msvc.WholeProgramOptimization = true;
                msvc.IntrinsicFunctions = true;
                msvc.FavorSizeOrSpeed = Msvc.FavorSizeOrSpeed.Speed;
                msvc.FunctionLevelLinking = true;
                msvc.RuntimeLibrary = Msvc.RuntimeLibrary.MultiThreadedDLL;
                break;
        }

        //if (configuration.Configuration != BuildConfiguration.Debug)
        //{
        //    configuration.AdditionalCompilerOptions.Add("/Qvec-report:2");
        //}
    }

    private static void _ConfigureLinker(CopiumCppConfiguration configuration, MsvcLibrarianOptions linker)
    {
        linker.SuppressStartupBanner = true;
        linker.LinkLibraryDependencies = false;
        linker.TreatLibWarningAsErrors = true;
        linker.TargetMachine = Msvc.TargetMachine.MachineX64;

        switch (configuration.Configuration)
        {
            case BuildConfiguration.Debug:
                linker.LinkTimeCodeGeneration = false;
                break;
            case BuildConfiguration.Development:
            case BuildConfiguration.Release:
                linker.LinkTimeCodeGeneration = true;
                break;
        }
    }

    private static void _ConfigureLinker(CopiumCppConfiguration configuration, MsvcLinkerOptions linker)
    {
        linker.SuppressStartupBanner = true;
        linker.IgnoreImportLibrary = false;
        linker.RegisterOutput = false;
        linker.PerUserRedirection = false;
        linker.LinkLibraryDependencies = true;
        linker.UseLibraryDependencyInputs = false;
        //linker.TreatLinkerWarningAsErrors = true;
        linker.SubSystem = Msvc.SubSystem.Console;
        linker.LargeAddressAware = true;
        linker.TargetMachine = Msvc.TargetMachine.MachineX64;

        switch (configuration.Configuration)
        {
            case BuildConfiguration.Debug:
                linker.LinkIncremental = true;
                linker.GenerateDebugInformation = Msvc.GenerateDebugInformation.DebugFastLink;
                linker.OptimizeReferences = false;
                linker.EnableCOMDATFolding = false;
                break;
            case BuildConfiguration.Development:
                linker.LinkIncremental = false;
                linker.GenerateDebugInformation = Msvc.GenerateDebugInformation.DebugFastLink;
                linker.OptimizeReferences = true;
                linker.EnableCOMDATFolding = false;
                linker.LinkTimeCodeGeneration = Msvc.LinkTimeCodeGeneration.UseFastLinkTimeCodeGeneration;
                break;
            case BuildConfiguration.Release:
                linker.LinkIncremental = false;
                linker.GenerateDebugInformation = Msvc.GenerateDebugInformation.False;
                linker.OptimizeReferences = true;
                linker.EnableCOMDATFolding = true;
                linker.LinkTimeCodeGeneration = Msvc.LinkTimeCodeGeneration.UseLinkTimeCodeGeneration;
                break;
        }
    }
}
