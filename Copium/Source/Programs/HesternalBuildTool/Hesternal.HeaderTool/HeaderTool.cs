using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using Hesternal.HeaderTool.Generator;
using Hesternal.HeaderTool.Parser;

namespace Hesternal.HeaderTool
{
    public readonly struct HeaderToolResult
    {
        public readonly FileInfo HppFile;
        public readonly FileInfo CppFile;

        public HeaderToolResult(FileInfo hppFile, FileInfo cppFile)
        {
            HppFile = hppFile;
            CppFile = cppFile;
        }

        public void Deconstruct(out FileInfo hppFile, out FileInfo cppFile)
        {
            hppFile = HppFile;
            cppFile = CppFile;
        }
    }


    public class HeaderToolTaskItem
    {
        public readonly FileInfo HeaderFile;

        public HeaderToolResult? Result { get; internal set; }

        public HeaderToolTaskItem(FileInfo headerFile)
        {
            HeaderFile = headerFile;
        }
    }


    public sealed class HeaderToolOptions
    {
        public readonly bool bDryRun;

        public readonly DirectoryInfo TargetDir;
        public readonly DirectoryInfo HesternalConfigDir;

        public readonly DirectoryInfo TargetGeneratedDir;
        public readonly string BaseGeneratedIncludeDir;


        public static HeaderToolOptions Create(DirectoryInfo targetDir, DirectoryInfo hesternalConfigDir, DirectoryInfo targetGeneratedDir, string baseGeneratedIncludeDir)
        {
            return new HeaderToolOptions(false, targetDir, hesternalConfigDir, targetGeneratedDir, baseGeneratedIncludeDir);
        }

        public static HeaderToolOptions CreateDryRun(DirectoryInfo targetDir, DirectoryInfo hesternalConfigDir)
        {
            return new HeaderToolOptions(true, targetDir, hesternalConfigDir, null, null);
        }


        private HeaderToolOptions(
            bool dryRun,
            DirectoryInfo targetDir, DirectoryInfo hesternalConfigDir,
            DirectoryInfo targetGeneratedDir, string baseGeneratedIncludeDir
            )
        {
            bDryRun = dryRun;
            TargetDir = targetDir;
            HesternalConfigDir = hesternalConfigDir;
            TargetGeneratedDir = targetGeneratedDir;
            BaseGeneratedIncludeDir = baseGeneratedIncludeDir;
        }
    }


    public sealed class HeaderTool
    {
        private const string k_GeneratedHppExtension = ".cht.hpp";
        private const string k_GeneratedCppExtension = ".cht.cpp";

        private static HesternalHeaderToolConfig s_Config;

        private readonly bool m_bDryRun;
        private readonly DirectoryInfo m_targetDir;
        private readonly DirectoryInfo m_generatedHeadersDir;
        public readonly DirectoryInfo GeneratedSourcesDir;


        public HeaderTool(HeaderToolOptions options)
        {
            m_bDryRun = options.bDryRun;
            m_targetDir = options.TargetDir;

            if (options.bDryRun)
            {
                m_generatedHeadersDir = null;
                GeneratedSourcesDir = null;
            }
            else
            {
                m_generatedHeadersDir = new DirectoryInfo(Path.Combine(options.TargetGeneratedDir.FullName, "Include", options.BaseGeneratedIncludeDir));
                GeneratedSourcesDir = new DirectoryInfo(Path.Combine(options.TargetGeneratedDir.FullName, "Source"));
            }

            s_Config = HesternalHeaderToolConfig.LoadConfig(options.HesternalConfigDir);
            EnumGenerator.SetConfig(s_Config.EnumGenerator);
            StructGenerator.SetConfig(s_Config.StructGenerator);
        }


        public void Generate(HeaderToolTaskItem[] taskItems)
        {
            if (m_bDryRun == false)
            {
                m_generatedHeadersDir.Create();
                GeneratedSourcesDir.Create();
            }

            foreach (HeaderToolTaskItem taskItem in taskItems)
            {
                FileInfo headerFile = taskItem.HeaderFile;

                CppHeaderDesc cppHeaderDesc = CppHeaderParser.Parse(headerFile);
                if (cppHeaderDesc == null)
                {
                    continue;
                }

                if (m_bDryRun == false)
                {
                    taskItem.Result = _GenerateSourcesForHeader(cppHeaderDesc);
                }
            }
        }


        private HeaderToolResult _GenerateSourcesForHeader(CppHeaderDesc cppHeaderDesc)
        {
            string headerName = Path.GetFileNameWithoutExtension(cppHeaderDesc.HeaderFile.Name);

            FileInfo generatedHppFile = _CreateHppFileInfo(headerName);
            FileInfo generatedCppFile = _CreateCppFileInfo(headerName);

            string chtFilePathMacro = _HeaderPathToChtFilePath(cppHeaderDesc.HeaderFile, m_targetDir.FullName.Length);

            using (var headerWriter = new CppWriter(generatedHppFile))
            {
                string[] includes = cppHeaderDesc.HasEnums ? EnumGenerator.HppIncludes : null;
                string[] moduleImports = cppHeaderDesc.HasStructsOrClasses ? StructGenerator.HppImports: null;
                headerWriter.WriteFileHeader_Hpp(includes, moduleImports);

                if (cppHeaderDesc.HasStructsOrClasses)
                {
                    headerWriter.WriteLine(
                        @"// NOTE(v.matushkin): Module consumer can't use serialization methods if I just forward declare IBinaryConverter.
//  So I either need to export forward declaration ""namespace Hesternal { export struct IBinaryConverter; }""
//   or import IBinaryConverter module.
//  Exporting forward declaration will break if module consumer also imports IBinaryConverter module.
//  So importing IBinaryConverter in the header seems like my only choice, although this potentially lead to
//   circular dependencies. Especially if I add something more than just serialization.");
                    headerWriter.WriteLine();
                }

                headerWriter.Write($"#define {s_Config.FilePathMacro} ");
                headerWriter.WriteLine(chtFilePathMacro);
                headerWriter.WriteLine();

                ClassGenerator.WriteHpp(cppHeaderDesc, headerWriter, chtFilePathMacro);
                StructGenerator.WriteHpp(cppHeaderDesc, headerWriter, chtFilePathMacro);
                EnumGenerator.WriteHpp(cppHeaderDesc, headerWriter, chtFilePathMacro);
            }

            using (var cppWriter = new CppWriter(generatedCppFile))
            {
                IEnumerable<string> stdIncludes = Enumerable.Empty<string>();
                IEnumerable<string> moduleImports = Enumerable.Empty<string>();

                if (cppHeaderDesc.HasStructsOrClasses)
                {
                    moduleImports = StructGenerator.CppImports;
                }
                if (cppHeaderDesc.HasEnums)
                {
                    stdIncludes = EnumGenerator.CppStdIncludes;
                    moduleImports = moduleImports.Concat(EnumGenerator.CppImports);
                }

                string[] stdIncludesArray = stdIncludes.Distinct().ToArray();
                string[] moduleImportsArray = moduleImports.Distinct().ToArray();

                Array.Sort(stdIncludesArray);
                Array.Sort(moduleImportsArray);

                cppWriter.WriteFileHeader_ModuleImplementation(cppHeaderDesc.Module, null, stdIncludesArray, moduleImportsArray);
                cppWriter.BeginNamespace(s_Config.Namespace);

                ClassGenerator.WriteCpp(cppHeaderDesc, cppWriter);
                StructGenerator.WriteCpp(cppHeaderDesc, cppWriter);
                EnumGenerator.WriteCpp(cppHeaderDesc, cppWriter);

                cppWriter.EndNamespace();
            }

            return new HeaderToolResult(generatedHppFile, generatedCppFile);
        }


        private FileInfo _CreateHppFileInfo(string headerName)
        {
            return _CreateFileInfo(m_generatedHeadersDir, headerName, k_GeneratedHppExtension);
        }

        private FileInfo _CreateCppFileInfo(string headerName)
        {
            return _CreateFileInfo(GeneratedSourcesDir, headerName, k_GeneratedCppExtension);
        }

        private static FileInfo _CreateFileInfo(DirectoryInfo dir, string headerName, string extension)
        {
            return new FileInfo(Path.Combine(dir.FullName, headerName + extension));
        }


        private static string _HeaderPathToChtFilePath(FileInfo headerFile, int projectPathLength)
        {
            string headerProjectRelativePath = headerFile.FullName.Substring(projectPathLength);

            string dir = Path.GetDirectoryName(headerProjectRelativePath);
            string[] dirParts = dir.Split(new[] { Path.DirectorySeparatorChar }, StringSplitOptions.RemoveEmptyEntries);
            string name = Path.GetFileNameWithoutExtension(headerFile.Name);
            string extension = headerFile.Extension.Substring(1); // Remove '.' from extension

            return $"{string.Join("_", dirParts)}_{name}_{extension}";
        }
    }
}
