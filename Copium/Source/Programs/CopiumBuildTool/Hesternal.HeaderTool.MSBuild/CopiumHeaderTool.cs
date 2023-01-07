using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using Hesternal.HeaderTool.Parser;

using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

// NOTE(v.matushkin): May be it's better to use TaskTool
//  https://natemcmaster.com/blog/2017/07/05/msbuild-task-in-nuget/
//  https://natemcmaster.com/blog/2017/11/11/msbuild-task-with-dependencies/
//  https://natemcmaster.com/blog/2017/11/11/build-tools-in-nuget/

// NOTE(v.matushkin): Some thoughts on how to prevent MSBuild from locking this assembly when executing task
//  https://github.com/dotnet/msbuild/issues/6461

namespace Hesternal.HeaderTool.MSBuild
{
    public sealed class HesternalHeaderTool : Task
    {
        private sealed class MSBuildHeaderToolTaskItem : HeaderToolTaskItem
        {
            public readonly ITaskItem HeaderItem;

            public MSBuildHeaderToolTaskItem(ITaskItem headerItem, FileInfo headerFile)
                : base(headerFile)
            {
                HeaderItem = headerItem;
            }
        }


        private const string k_TaskName = "HesternalHeaderTool";
        private const string k_TLogNamePrefix = k_TaskName;
        private const string k_TLogReadFileName = k_TLogNamePrefix + ".read.1u.tlog";
        private const string k_TLogWriteFileName = k_TLogNamePrefix + ".write.1u.tlog";

        private DirectoryInfo m_hesternalConfigDir;
        private DirectoryInfo m_targetDir;
        private DirectoryInfo m_targetGeneratedDir;

        private string m_tlogReadPath;
        private string m_tlogWritePath;
        private CanonicalTrackedOutputFiles m_outputFiles;

        private string m_msbuildDummyCppName;
        private FileInfo m_msbuildDummyCppFile;


        [Required] public string HesternalConfigDir { get; set; }
        [Required] public string TargetName { get; set; }
        [Required] public string TargetDir { get; set; }
        [Required] public string TargetGeneratedDir { get; set; }
        [Required] public string BaseGeneratedIncludeDir { get; set; }
        [Required] public ITaskItem[] Headers { get; set; }
        [Required] public string TLogDirectory { get; set; }

        /// <summary>
        /// Identity = Header path
        [Output] public ITaskItem[] GeneratedFiles { get; private set; }


        public override bool Execute()
        {
            try
            {
                _LogImportantMessage("Running " + k_TaskName + " for {0}", TargetName);

                m_hesternalConfigDir = new DirectoryInfo(HesternalConfigDir);
                if (m_hesternalConfigDir.Exists == false)
                {
                    Log.LogError("TargetDir doesn't exist: {0}", HesternalConfigDir);
                    return false;
                }

                m_targetDir = new DirectoryInfo(TargetDir);
                if (m_targetDir.Exists == false)
                {
                    Log.LogError("TargetDir doesn't exist: {0}", TargetDir);
                    return false;
                }

                m_targetGeneratedDir = new DirectoryInfo(TargetGeneratedDir);
                if (m_targetGeneratedDir.Exists == false)
                {
                    m_targetGeneratedDir.Create();
                }

                m_msbuildDummyCppName = $"msbuild_{TargetName.ToLower()}_dummy.cpp";

                m_tlogReadPath = Path.Combine(TLogDirectory, k_TLogReadFileName);
                m_tlogWritePath = Path.Combine(TLogDirectory, k_TLogWriteFileName);

                MSBuildHeaderToolTaskItem[] headersToProcess = _ComputeHeadersToProcess();

                if (headersToProcess.Length != 0)
                {
                    if (_ValidateHeaders(headersToProcess) == false)
                    {
                        return false;
                    }

                    _RunHeaderTool(headersToProcess);

                    _SaveTLogRead();
                    //_SaveTLogWrite();
                    m_outputFiles.SaveTlog();
                }
                else
                {
                    _LogImportantMessage("\tNothing changed, skipping generation");
                }

                // NOTE(v.matushkin): Need to get all generated files every time,
                //  cause I don't know how to add <ClCompile> to the project permanently, if it's even possible.
                GeneratedFiles = _GetGeneratedFiles();
            }
            catch (ParserException e)
            {
                Log.LogError(k_TaskName, string.Empty, string.Empty, e.FilePath, e.Line, e.Column, 0, 0, e.Message);
                // NOTE(v.matushkin): Sometimes needed for debug
                Log.LogErrorFromException(e, true);
                return false;
            }
            catch (Exception e)
            {
                Log.LogErrorFromException(e, true);
                return false;
            }

            return true;
        }


        private MSBuildHeaderToolTaskItem[] _ComputeHeadersToProcess()
        {
            // Key - tlog file path in upper case, Value - normal file path
            Dictionary<string, string> tlogNameToNormalName = Headers.ToDictionary(
                headerItem => headerItem.ItemSpec.ToUpperInvariant(),
                headerItem => headerItem.ItemSpec);
            ITaskItem[] sourceFiles = tlogNameToNormalName.Keys.Select(headerPath => new TaskItem(headerPath)).ToArray();

            var tlogRead = new TaskItem(m_tlogReadPath);
            var tlogWrite = new TaskItem(m_tlogWritePath);
            // TaskItem tlogCommand = new TaskItem(Path.Combine(TLogDirectory, k_TLogNamePrefix + ".command.1.tlog"));

            m_outputFiles = new CanonicalTrackedOutputFiles(this, new[] { tlogWrite }, true);
            var inputFiles = new CanonicalTrackedInputFiles(
                this, new[] { tlogRead }, sourceFiles, null, m_outputFiles, true, false);

            return inputFiles.ComputeSourcesNeedingCompilation()
                .Select(headerItem => new MSBuildHeaderToolTaskItem(headerItem, new FileInfo(tlogNameToNormalName[headerItem.ItemSpec])))
                .ToArray();
        }

        private bool _ValidateHeaders(MSBuildHeaderToolTaskItem[] headerTasks)
        {
            foreach (MSBuildHeaderToolTaskItem headerTask in headerTasks)
            {
                FileInfo headerFile = headerTask.HeaderFile;

                if (string.Compare(headerFile.Extension, ".ixx", true) != 0)
                {
                    Log.LogError("Header file extension wasn't '.ixx': {0}", headerFile.FullName);
                    return false;
                }
                if (headerFile.Exists == false)
                {
                    Log.LogError("Header file doesn't exist: {0}", headerFile.FullName);
                    return false;
                }
            }

            return true;
        }

        private void _RunHeaderTool(MSBuildHeaderToolTaskItem[] headerTasks)
        {
            var chtGenerator = new HeaderTool(HeaderToolOptions.Create(m_targetDir, m_hesternalConfigDir, m_targetGeneratedDir, BaseGeneratedIncludeDir));
            chtGenerator.Generate(headerTasks);

            m_msbuildDummyCppFile = new FileInfo(Path.Combine(chtGenerator.GeneratedSourcesDir.FullName, m_msbuildDummyCppName));

            foreach (MSBuildHeaderToolTaskItem headerTask in headerTasks)
            {
                FileInfo headerFile = headerTask.HeaderFile;

                m_outputFiles.RemoveEntriesForSource(headerTask.HeaderItem);

                if (headerTask.Result.HasValue)
                {
                    var (generatedHppFile, generatedCppFile) = headerTask.Result.Value;
                    m_outputFiles.AddComputedOutputsForSourceRoot(headerFile.FullName, new[] { generatedHppFile.FullName, generatedCppFile.FullName });

                    _LogImportantMessage("\tGenerated for: {0}\n\t\t{1}\n\t\t{2}", headerFile.FullName, generatedHppFile.FullName, generatedCppFile.FullName);
                }
                else
                {
                    m_outputFiles.AddComputedOutputForSourceRoot(headerFile.FullName, m_msbuildDummyCppFile.FullName);

                    _LogImportantMessage("\tSkipped for: {0}", headerFile.FullName);
                }
            }
        }

        // NOTE(v.matushkin): This is some quality method right here
        private ITaskItem[] _GetGeneratedFiles()
        {
            bool isDummyFileNeeded = false;
            var generatedFiles = new List<ITaskItem>();

            // NOTE(v.matushkin): Not sure if there is a better way to find out if dummy file is needed
            foreach (Dictionary<string, DateTime> headerGeneratedFiles in m_outputFiles.DependencyTable.Values)
            {
                int foundMsbuildDummy = 0;
                int foundGeneratedCpp = 0;
                int foundGeneratedHpp = 0;

                foreach (string generatedFile in headerGeneratedFiles.Keys)
                {
                    string fileName = Path.GetFileName(generatedFile).ToLower();
                    string extension = Path.GetExtension(fileName);

                    if (extension == ".hpp")
                    {
                        ++foundGeneratedHpp;
                    }
                    else if (extension == ".cpp")
                    {
                        if (fileName.Equals(m_msbuildDummyCppName, StringComparison.OrdinalIgnoreCase))
                        {
                            ++foundMsbuildDummy;
                        }
                        else
                        {
                            ++foundGeneratedCpp;
                            generatedFiles.Add(new TaskItem(generatedFile));
                        }
                    }
                    else
                    {
                        throw new Exception($"This shouldn't happen, but found some weird file: {generatedFile}");
                    }
                }

                if (foundMsbuildDummy != 0)
                {
                    if (foundGeneratedCpp != 0 || foundGeneratedHpp != 0)
                    {
                        throw new Exception("Shit happend");
                    }
                    if (foundMsbuildDummy != 1)
                    {
                        throw new Exception("Shit happend");
                    }

                    isDummyFileNeeded = true;

                    continue;
                }

                if (foundGeneratedCpp != 1 || foundGeneratedHpp != 1)
                {
                    throw new Exception("Shit happend");
                }
            }

            if (m_msbuildDummyCppFile != null)
            {
                if (isDummyFileNeeded)
                {
                    File.WriteAllText(m_msbuildDummyCppFile.FullName, "\n");
                }
                else if (isDummyFileNeeded && m_msbuildDummyCppFile.Exists)
                {
                    m_msbuildDummyCppFile.Delete();
                }
            }

            return generatedFiles.ToArray();
        }

        private void _SaveTLogRead()
        {
            using (var tlogReadWriter = new StreamWriter(m_tlogReadPath, false, System.Text.Encoding.Unicode))
            {
                foreach (ITaskItem header in Headers)
                {
                    tlogReadWriter.Write('^');
                    tlogReadWriter.WriteLine(header.GetMetadata("FullPath").ToUpperInvariant());
                }
            }
        }

        // private void _SaveTLogWrite()
        // {
        //     using (var tlogReadWriter = new StreamWriter(m_tlogReadPath, false, System.Text.Encoding.Unicode))
        //     {
        //         foreach (ITaskItem header in Headers)
        //         {
        //             tlogReadWriter.Write('^');
        //             tlogReadWriter.WriteLine(header.ItemSpec.ToUpperInvariant());
        //         }
        //     }
        // }


        private void _LogImportantMessage(string message, params object[] messageArgs)
        {
            Log.LogMessage(MessageImportance.High, message, messageArgs);
        }
    }
}
