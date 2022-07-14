// NOTE(v.matushkin): Not sure if I need a dedicated class for .props files

// using Copium.BuildTool.Core;
// 
// namespace Copium.BuildTool.Project.VisualStudio;
// 
// 
// internal readonly struct VsXmlImportElement
// {
//     public readonly string Project;
//     public readonly string Condition;
// 
//     public VsXmlImportElement(string project, string condition)
//     {
//         Project = project;
//         Condition = condition;
//     }
// }
// 
// internal readonly struct VsXmlElement
// {
//     public readonly string Name;
//     public readonly NameValue[] ChildElements;
// 
//     public VsXmlElement(string name, NameValue[] childElements)
//     {
//         Name = name;
//         ChildElements = childElements;
//     }
// }
// 
// internal readonly struct VsXmlElementGroup
// {
//     public readonly string Condition;
//     public readonly VsXmlElement[] Elements;
// 
//     public VsXmlElementGroup(string condition, VsXmlElement[] elements)
//     {
//         Condition = condition;
//         Elements = elements;
//     }
// }
// 
// internal sealed class VsPropsFileCreateInfo
// {
//     /// <summary>
//     /// PropertySheets ImportGroup
//     /// </summary>
//     public VsXmlImportElement[]? PropertySheets;
//     /// <summary>
//     /// UserMacros PropertyGroup
//     /// </summary>
//     public NameValue[]? UserMacros;
//     /// <summary>
//     /// Per-configuration ItemDefinitionGroups
//     /// </summary>
//     public VsXmlElementGroup[]? ConfigurationItemDefinitions;
// }
// 
// 
// internal sealed class VsPropsFile
// {
//     private readonly FileItem m_file;
// 
//     public string FullPath => m_file.FullPath;
//     public string FileName => m_file.FileName;
// 
// 
//     public VsPropsFile(FileItem propsFile)
//     {
//         m_file = propsFile;
//     }
// 
// 
//     public static VsPropsFile Create(FileItem propsFile, VsPropsFileCreateInfo createInfo)
//     {
//         using (VsXmlWriter propsFileWriter = new(propsFile))
//         {
//             propsFileWriter.BeginProject(null, null, null, true);
// 
//             if (createInfo.PropertySheets != null && createInfo.PropertySheets.Length != 0)
//             {
//                 propsFileWriter.BeginImportGroup("PropertySheets");
//                 foreach (VsXmlImportElement propertySheet in createInfo.PropertySheets)
//                 {
//                     propsFileWriter.Import(propertySheet.Project, null, propertySheet.Condition);
//                 }
//                 propsFileWriter.EndElement();
//             }
// 
//             if (createInfo.UserMacros != null && createInfo.UserMacros.Length != 0)
//             {
//                 propsFileWriter.BeginPropertyGroup("UserMacros");
//                 foreach (NameValue userMacro in createInfo.UserMacros)
//                 {
//                     propsFileWriter.Property(userMacro.Name, userMacro.Value);
//                 }
//                 propsFileWriter.EndElement();
//             }
// 
//             if (createInfo.ConfigurationItemDefinitions != null && createInfo.ConfigurationItemDefinitions.Length != 0)
//             {
//                 foreach (VsXmlElementGroup elementGroup in createInfo.ConfigurationItemDefinitions)
//                 {
//                     propsFileWriter.BeginItemDefinitionGroup(elementGroup.Condition);
//                     foreach (VsXmlElement element in elementGroup.Elements)
//                     {
//                         propsFileWriter.BeginItem(element.Name);
//                         foreach (NameValue childElemnt in element.ChildElements)
//                         {
//                             propsFileWriter.Property(childElemnt.Name, childElemnt.Value);
//                         }
//                         propsFileWriter.EndElement();
//                     }
//                     propsFileWriter.EndElement();
//                 }
//             }
// 
//             propsFileWriter.EndElement();
//         }
// 
//         return new VsPropsFile(propsFile);
//     }
// }
