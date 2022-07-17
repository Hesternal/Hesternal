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
        public readonly DirectoryInfo CopiumConfigDir;

        public readonly DirectoryInfo TargetGeneratedDir;
        public readonly string BaseGeneratedIncludeDir;


        public static HeaderToolOptions Create(DirectoryInfo targetDir, DirectoryInfo copiumConfigDir, DirectoryInfo targetGeneratedDir, string baseGeneratedIncludeDir)
        {
            return new HeaderToolOptions(false, targetDir, copiumConfigDir, targetGeneratedDir, baseGeneratedIncludeDir);
        }

        public static HeaderToolOptions CreateDryRun(DirectoryInfo targetDir, DirectoryInfo copiumConfigDir)
        {
            return new HeaderToolOptions(true, targetDir, copiumConfigDir, null, null);
        }


        private HeaderToolOptions(
            bool dryRun,
            DirectoryInfo targetDir, DirectoryInfo copiumConfigDir,
            DirectoryInfo targetGeneratedDir, string baseGeneratedIncludeDir
            )
        {
            bDryRun = dryRun;
            TargetDir = targetDir;
            CopiumConfigDir = copiumConfigDir;
            TargetGeneratedDir = targetGeneratedDir;
            BaseGeneratedIncludeDir = baseGeneratedIncludeDir;
        }
    }


    public sealed class HeaderTool
    {
        private const string k_GeneratedHppExtension = ".cht.hpp";
        private const string k_GeneratedCppExtension = ".cht.cpp";

        private static CopiumHeaderToolConfig s_Config;

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

            s_Config = CopiumHeaderToolConfig.LoadConfig(options.CopiumConfigDir);
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

                string[] stdIncludesArray = stdIncludes.Distinct().ToArray();
                string[] moduleImportsArray = moduleImports.Distinct().ToArray();

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
