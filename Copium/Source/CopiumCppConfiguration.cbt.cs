using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.CompilerServices;

using Copium.BuildTool;
using Copium.Core.Net7;


internal enum BuildConfiguration
{
    Debug,
    Development,
    Release,
}

internal sealed class CopiumCppConfigurationDesc : CppConfigurationDesc
{
    public required BuildConfiguration Configuration { get; init; }

    public override string Name => Compiler.ToString().ToLower() + "-" + Configuration.ToString().ToLower();
    public override OutputType OutputType => m_outputType;


    private readonly OutputType m_outputType;


    public CopiumCppConfigurationDesc(OutputType outputType)
    {
        m_outputType = outputType;
    }
}

internal sealed class CopiumCppConfigurationMatrix : ICppConfigurationMatrix
{
    public readonly BuildOS[] OS = { BuildOS.Windows };
    public readonly BuildCompiler[] Compiler = { BuildCompiler.Msvc };
    public readonly BuildPlatform[] Platform = { BuildPlatform.x64 };
    public readonly BuildConfiguration[] Configuration =
    {
        BuildConfiguration.Debug,
        BuildConfiguration.Development,
        BuildConfiguration.Release,
    };

    public readonly OutputType OutputType;


    public CopiumCppConfigurationMatrix(OutputType outputType)
    {
        OutputType = outputType;
    }


    // TODO(v.matushkin): This shit should be replaced by source generators, but I'm seek from this BuildTool already
    public ReadOnlyCollection<CppProjectConfiguration> MakeConfigurations()
    {
        int[] m_length = { 1, 1, 1, 3 };
        int[] m_indices = new int[4];
        m_indices[0] = -1;

        int numConfigurations = 1;

        for (int i = 0; i < m_length.Length; i++)
        {
            numConfigurations *= m_length[i];
        }

        CppProjectConfiguration[] configurations = new CppProjectConfiguration[numConfigurations];

        for (int i = 0; i < numConfigurations; i++)
        {
            for (int j = 0; j < m_indices.Length; j++)
            {
                if (++m_indices[j] >= m_length[j])
                {
                    m_indices[j] = 0;
                }
                else
                {
                    break;
                }
            }

            configurations[i] = new CopiumCppConfiguration(new CopiumCppConfigurationDesc(OutputType)
            {
                OS = OS[m_indices[0]],
                Compiler = Compiler[m_indices[1]],
                Platform = Platform[m_indices[2]],
                Configuration = Configuration[m_indices[3]],
            });
        }

        return Array.AsReadOnly(configurations);
    }
}



internal sealed class CopiumCppConfiguration : CppProjectConfiguration
{
    public readonly BuildConfiguration Configuration;


    public CopiumCppConfiguration(CopiumCppConfigurationDesc desc)
        : base(desc)
    {
        Configuration = desc.Configuration;
    }
}
