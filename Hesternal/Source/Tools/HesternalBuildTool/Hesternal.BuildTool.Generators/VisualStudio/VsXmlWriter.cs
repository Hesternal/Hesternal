using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;

using Hesternal.Core.Net7;

namespace Hesternal.BuildTool.Generators.VisualStudio;


internal sealed class VsXmlWriter : IDisposable
{
    private const string k_DefaultToolsVersion = "4.0";
    private const string k_Xmlns = "http://schemas.microsoft.com/developer/msbuild/2003";

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

        if (sdk is not null)
        {
            m_xmlWriter.WriteAttributeString(VsXmlAttribute.Sdk, sdk);
        }

        toolsVersion ??= k_DefaultToolsVersion;
        m_xmlWriter.WriteAttributeString(VsXmlAttribute.ToolsVersion, toolsVersion);

        if (defaultTargets is not null)
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

    public void BeginImportGroup(string label, string? condition = null)
    {
        m_xmlWriter.WriteStartElement(VsXmlItem.ImportGroup);
        m_xmlWriter.WriteAttributeString(VsXmlAttribute.Label, label);
        if (condition is not null)
        {
            m_xmlWriter.WriteAttributeString(VsXmlAttribute.Condition, condition);
        }
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
        if (label is not null)
        {
            m_xmlWriter.WriteAttributeString(VsXmlAttribute.Label, label);
        }
        if (condition is not null)
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
        if (value.Length > 0)
        {
            m_xmlWriter.WriteElementString(name, value);
        }
    }

    public void Property(string name, bool value)
    {
        m_xmlWriter.WriteStartElement(name);
        m_xmlWriter.WriteValue(value);
        m_xmlWriter.WriteEndElement();
    }

    /// <summary>
    /// Adds $(<paramref name="name"/>) to the values at the end<br/>
    /// Doesn't write anything if the <paramref name="values"/> are empty
    /// </summary>
    public void Property(string name, IEnumerable<string> values, string separator = ";")
    {
        if (values.Any())
        {
            m_xmlWriter.WriteStartElement(name);
            _WritePropertyValues(name, separator, values);
            m_xmlWriter.WriteEndElement();
        }
    }

    public void Import(string project, string? condition = null, string? label = null)
    {
        m_xmlWriter.WriteStartElement(VsXmlItem.Import);
        m_xmlWriter.WriteAttributeString(VsXmlAttribute.Project, project);
        if (label is not null)
        {
            m_xmlWriter.WriteAttributeString(VsXmlAttribute.Label, label);
        }
        if (condition is not null)
        {
            m_xmlWriter.WriteAttributeString(VsXmlAttribute.Condition, condition);
        }
        m_xmlWriter.WriteEndElement();
    }

    public void ProjectGuid(string guid)
    {
        Property(VsXmlItem.ProjectGuid, guid);
    }


    private void _WritePropertyValues(string name, string separator, IEnumerable<string> values)
    {
        // NOTE(v.matushkin): Using WriteString to write char, cause WriteCharEntity writes some unicode shit,
        //  although it works, I just prefer to avoid it for aesthetic reasons.

        foreach (string value in values)
        {
            m_xmlWriter.WriteString(value);
            m_xmlWriter.WriteString(separator);
        }

        m_xmlWriter.WriteString("%(");
        m_xmlWriter.WriteString(name);
        m_xmlWriter.WriteString(")");
    }
}
