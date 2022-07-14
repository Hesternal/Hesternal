using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Xml;

using Copium.BuildTool.Core;
using Copium.Core;

namespace Copium.BuildTool.Project.VisualStudio;


internal sealed class VsXmlWriter : IDisposable
{
    private const string k_DefaultToolsVersion = "4.0";
    private const string k_Xmlns = "http://schemas.microsoft.com/developer/msbuild/2003";

    private readonly Lazy<StringBuilder> m_stringBuilder = new(LazyThreadSafetyMode.None);
    private readonly XmlWriter m_xmlWriter;


    public VsXmlWriter(FileItem file)
        : this(file.OpenWriteText())
    {
    }

    public VsXmlWriter(ProjectFile file)
        : this(file.OpenWrite())
    {
    }

    private VsXmlWriter(StreamWriter fileWriter)
    {
        XmlWriterSettings settings = new()
        {
            Indent = true,
            Encoding = Encoding.UTF8,
            CloseOutput = true,
        };
        m_xmlWriter = XmlWriter.Create(fileWriter, settings);
    }

    public void Dispose() => m_xmlWriter.Dispose();


    public void BeginProject(string? sdk, string? toolsVersion, string? defaultTargets, bool bWriteXmlns)
    {
        m_xmlWriter.WriteStartElement(VsXmlItem.Project, bWriteXmlns ? k_Xmlns : null);

        if (sdk != null)
        {
            m_xmlWriter.WriteAttributeString(VsXmlAttribute.Sdk, sdk);
        }

        toolsVersion ??= k_DefaultToolsVersion;
        m_xmlWriter.WriteAttributeString(VsXmlAttribute.ToolsVersion, toolsVersion);

        if (defaultTargets != null)
        {
            m_xmlWriter.WriteAttributeString(VsXmlAttribute.DefaultTargets, defaultTargets);
        }
    }

    public void BeginItem(string name)
    {
        m_xmlWriter.WriteStartElement(name);
    }

    public void BeginItem(string name, string include)
    {
        m_xmlWriter.WriteStartElement(name);
        m_xmlWriter.WriteAttributeString(VsXmlAttribute.Include, include);
    }

    public void BeginImportGroup(string label)
    {
        m_xmlWriter.WriteStartElement(VsXmlItem.ImportGroup);
        m_xmlWriter.WriteAttributeString(VsXmlAttribute.Label, label);
    }

    public void BeginItemDefinitionGroup()
    {
        m_xmlWriter.WriteStartElement(VsXmlItem.ItemDefinitionGroup);
    }

    public void BeginItemDefinitionGroup(string condition)
    {
        m_xmlWriter.WriteStartElement(VsXmlItem.ItemDefinitionGroup);
        m_xmlWriter.WriteAttributeString(VsXmlAttribute.Condition, condition);
    }

    public void BeginItemGroup()
    {
        m_xmlWriter.WriteStartElement(VsXmlItem.ItemGroup);
    }

    public void BeginItemGroup(string label)
    {
        m_xmlWriter.WriteStartElement(VsXmlItem.ItemGroup);
        m_xmlWriter.WriteAttributeString(VsXmlAttribute.Label, label);
    }

    public void BeginPropertyGroup(string? label = null, string? condition = null)
    {
        m_xmlWriter.WriteStartElement(VsXmlItem.PropertyGroup);
        if (label != null)
        {
            m_xmlWriter.WriteAttributeString(VsXmlAttribute.Label, label);
        }
        if (condition != null)
        {
            m_xmlWriter.WriteAttributeString(VsXmlAttribute.Condition, condition);
        }
    }


    public void EndElement()
    {
        m_xmlWriter.WriteEndElement();
    }


    public void Attribute(string name, string value)
{
        m_xmlWriter.WriteAttributeString(name, value);
    }


    public void Item(string name)
    {
        m_xmlWriter.WriteElementString(name, null);
    }

    public void Item(string name, string include)
    {
        m_xmlWriter.WriteStartElement(name);
        m_xmlWriter.WriteAttributeString(VsXmlAttribute.Include, include);
        m_xmlWriter.WriteEndElement();
    }

    public void Item(string name, string attribute, string attributeValue)
    {
        m_xmlWriter.WriteStartElement(name);
        m_xmlWriter.WriteAttributeString(attribute, attributeValue);
        m_xmlWriter.WriteEndElement();
    }

    public void Property(string name, string value)
    {
        m_xmlWriter.WriteElementString(name, value);
    }

    /// <summary>
    /// Adds $(<paramref name="name"/>) to the values at the end<br/>
    /// Doesn't write anything if the <paramref name="values"/> are empty
    /// </summary>
    public void Property(string name, IEnumerable<string> values)
    {
        if (values.Any())
        {
            m_xmlWriter.WriteElementString(name, MakePropertyValue(name, values));
        }
    }

    public void Import(string project, string? label = null, string? condition = null)
    {
        m_xmlWriter.WriteStartElement(VsXmlItem.Import);
        m_xmlWriter.WriteAttributeString(VsXmlAttribute.Project, project);
        if (label != null)
        {
            m_xmlWriter.WriteAttributeString(VsXmlAttribute.Label, label);
        }
        if (condition != null)
        {
            m_xmlWriter.WriteAttributeString(VsXmlAttribute.Condition, condition);
        }
        m_xmlWriter.WriteEndElement();
    }

    public void ProjectGuid(string guid)
    {
        Property(VsXmlItem.ProjectGuid, guid);
    }


    public string MakePropertyValue(string propertyName, IEnumerable<string> values)
    {
        StringBuilder stringBuilder = m_stringBuilder.Value;

        stringBuilder.AppendJoin(';', values);
        stringBuilder.Append(';');
        stringBuilder.Append("%(").Append(propertyName).Append(')');

        return stringBuilder.ToStringAndClear();
    }
}
