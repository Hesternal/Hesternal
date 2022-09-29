//using System;
//
//namespace Copium.BuildTool.Generators.VisualStudio;
//
//
//internal sealed class VsPropsFileGenerator : IDisposable
//{
//    private readonly VsPropsFile m_propsFile;
//
//    private readonly VsXmlWriter m_propsFileWriter;
//
//
//    public static void Generate(VsPropsFile propsFile)
//    {
//        using VsPropsFileGenerator generator = new(propsFile);
//        generator._GeneratePropsFile();
//    }
//
//
//    private VsPropsFileGenerator(VsPropsFile propsFile)
//    {
//        m_propsFile = propsFile;
//        m_propsFileWriter = new(propsFile.PropsFile);
//    }
//
//    public void Dispose() => m_propsFileWriter.Dispose();
//
//
//    private void _GeneratePropsFile()
//    {
//        m_propsFileWriter.BeginProject(null, null, null, true);
//
//        m_propsFileWriter.BeginPropertyGroup();
//
//        if (m_propsFile.ImportConditionVariable != null)
//        {
//            m_propsFileWriter.Property(m_propsFile.ImportConditionVariable, VsXmlPropertyValue.True);
//        }
//
//        foreach ((string name, string value) in m_propsFile.Base.Variables)
//        {
//            m_propsFileWriter.Property(name, value);
//        }
//
//        m_propsFileWriter.EndElement();
//
//        m_propsFileWriter.EndElement();
//    }
//}
