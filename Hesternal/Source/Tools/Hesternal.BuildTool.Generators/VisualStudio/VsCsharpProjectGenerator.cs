//using System;
//
//using Hesternal.BuildTool.Build;
//using Hesternal.CSharp.Net7;
//
//namespace Hesternal.BuildTool.Generators.VisualStudio;
//
//
//internal sealed class VsCsharpProjectGenerator : IDisposable
//{
//    private readonly RulesTarget m_rulesTarget;
//    /// <summary>
//    /// Hesternal.BuildTool.BuildRules.csproj project file
//    /// </summary>
//    private readonly VsCsharpProjectFile m_projectReferenceFile;
//
//    private readonly VsCsharpProjectFile m_projectFile;
//    private DirectoryItem ProjectDir => m_projectFile.ProjectDir;
//
//    private readonly VsXmlWriter m_projectWriter;
//
//
//    public static void Generate(VsGenerationContext generationContext, RulesTarget rulesTarget)
//    {
//        using VsCsharpProjectGenerator generator = new(generationContext, rulesTarget);
//        generator._GenerateProject();
//    }
//
//
//    private VsCsharpProjectGenerator(VsGenerationContext generationContext, RulesTarget rulesTarget)
//    {
//        m_rulesTarget = rulesTarget;
//        // NOTE(v.matushkin): Probably RulesTarget class should have a TargetReference field, just like CppTarget
//        m_projectReferenceFile = generationContext.GetCsharpProject("Hesternal.BuildTool.BuildRules");
//
//        m_projectFile = generationContext.GetCsharpProject(rulesTarget.TargetName);
//
//        m_projectWriter = new VsXmlWriter(m_projectFile);
//    }
//
//    public void Dispose() => m_projectWriter.Dispose();
//
//
//    private void _GenerateProject()
//    {
//        m_projectWriter.BeginProject(
//            VisualStudioConfig.Csproj.Sdk,
//            VisualStudioConfig.Csproj.ToolsVersion,
//            VisualStudioConfig.Csproj.DefaultTargets,
//            false);
//
//        //- PropertyGroup
//        {
//            m_projectWriter.BeginPropertyGroup();
//
//            // NOTE(v.matushkin): Doesn't work? Not for .csproj projects?
//            //  csprojPropertyGroup.AddProperty("ResolveNugetPackages", "false");
//
//            foreach (NameValue property in VisualStudioConfig.Csproj.Properties)
//            {
//                m_projectWriter.Property(property.Name, property.Value);
//            }
//
//            m_projectWriter.EndElement();
//        }
//
//        //- ProjectReference ItemGroup
//        {
//            m_projectWriter.BeginItemGroup();
//
//            m_projectWriter.BeginItem(VsXmlItem.ProjectReference, m_projectReferenceFile.GetRelativePath(ProjectDir));
//            m_projectWriter.Property(CsprojProjectReferenceProperty.Private, VsXmlPropertyValue.False);
//            m_projectWriter.EndElement();
//
//            m_projectWriter.EndElement();
//        }
//
//        //- Compile ItemGroup
//        {
//            m_projectWriter.BeginItemGroup();
//
//            foreach (FileItem sourceFile in m_rulesTarget.SourceFiles)
//            {
//                m_projectWriter.Item(CsprojItem.Compile, sourceFile.GetRelativePath(ProjectDir));
//            }
//
//            m_projectWriter.EndElement();
//        }
//
//        m_projectWriter.EndElement(); // BeginProject
//    }
//}
