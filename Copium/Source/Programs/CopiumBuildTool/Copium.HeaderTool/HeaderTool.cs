using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using Copium.HeaderTool.Generator;
using Copium.HeaderTool.Parser;

namespace Copium.HeaderTool
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


    public abstract class HeaderToolTaskItem
    {
        public HeaderToolResult? Result { get; internal set; }

        public abstract FileInfo HeaderFile { get; }
    }


    public sealed class HeaderTool
    {
        private const string k_GeneratedHppExtension = ".cht.hpp";
        private const string k_GeneratedCppExtension = ".cht.cpp";

        private static CopiumHeaderToolConfig s_Config;

        private readonly DirectoryInfo m_targetDir;
        private readonly DirectoryInfo m_generatedHeadersDir;
        public readonly DirectoryInfo GeneratedSourcesDir;


        public HeaderTool(
            DirectoryInfo targetDir, DirectoryInfo targetGeneratedDir, string baseGeneratedIncludeDir,
            DirectoryInfo copiumConfigDir
            )
        {
            m_targetDir = targetDir;
            m_generatedHeadersDir = new DirectoryInfo(Path.Combine(targetGeneratedDir.FullName, "Include", baseGeneratedIncludeDir));
            GeneratedSourcesDir = new DirectoryInfo(Path.Combine(targetGeneratedDir.FullName, "Source"));

            s_Config = CopiumHeaderToolConfig.LoadConfig(copiumConfigDir);
            EnumGenerator.SetConfig(s_Config.EnumGenerator);
            StructGenerator.SetConfig(s_Config.StructGenerator);
        }


        public void Generate(HeaderToolTaskItem[] taskItems)
        {
            m_generatedHeadersDir.Create();
            GeneratedSourcesDir.Create();

            foreach (HeaderToolTaskItem taskItem in taskItems)
            {
                FileInfo headerFile = taskItem.HeaderFile;

                CppHeaderDesc cppHeaderDesc = CppHeaderParser.Parse(headerFile);
                if (cppHeaderDesc == null)
                {
                    continue;
                }

                if (cppHeaderDesc.Enums.Count != 0 || cppHeaderDesc.Structs.Count != 0)
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
                string[] includes = cppHeaderDesc.Enums.Count != 0 ? EnumGenerator.HppIncludes : null;
                headerWriter.WriteFileHeader_Hpp(includes);

                headerWriter.Write("#define CHT_FILE_PATH ");
                headerWriter.WriteLine(chtFilePathMacro);
                headerWriter.WriteLine();

                if (cppHeaderDesc.Structs.Count != 0)
                {
                    headerWriter.WriteLine("namespace Copium { struct IBinaryConverter; }");
                    headerWriter.WriteLine();

                    StructGenerator.WriteHpp(cppHeaderDesc, headerWriter, chtFilePathMacro);
                }

                if (cppHeaderDesc.Enums.Count != 0)
                {
                    EnumGenerator.WriteHpp(cppHeaderDesc, headerWriter, chtFilePathMacro);
                }
            }

            using (var cppWriter = new CppWriter(generatedCppFile))
            {
                IEnumerable<string> stdIncludes = Enumerable.Empty<string>();
                IEnumerable<string> moduleImports = Enumerable.Empty<string>();

                if (cppHeaderDesc.Structs.Count != 0)
                {
                    moduleImports = StructGenerator.CppImports;
                }
                if (cppHeaderDesc.Enums.Count != 0)
                {
                    stdIncludes = EnumGenerator.CppStdIncludes;
                    moduleImports = moduleImports.Concat(EnumGenerator.CppImports);
                }

                var stdIncludesArray = stdIncludes.Distinct().ToArray();
                var moduleImportsArray = moduleImports.Distinct().ToArray();

                Array.Sort(stdIncludesArray);
                Array.Sort(moduleImportsArray);

                cppWriter.WriteFileHeader_ModuleImplementation(cppHeaderDesc.Module, null, stdIncludesArray, moduleImportsArray);
                cppWriter.BeginNamespace("Copium");

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
