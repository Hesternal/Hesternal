﻿using System;
using System.IO;
using System.Text.Json;

namespace Hesternal.HeaderTool.Generator
{
    internal sealed class EnumGeneratorConfig
    {
        public sealed class EnumGeneratorHppConfig
        {
            public readonly string[] Includes;

            public EnumGeneratorHppConfig(string[] includes)
            {
                Includes = includes;
            }
        }

        public sealed class EnumGeneratorCppConfig
        {
            public readonly string[] StdIncludes;
            public readonly string[] Imports;
            public readonly uint ToStringMapThreshold;
            public readonly uint FromStringMapThreshold;

            public EnumGeneratorCppConfig(
                string[] stdIncludes,
                string[] imports,
                uint toStringMapThreshold,
                uint fromStringMapThreshold
                )
            {
                StdIncludes = stdIncludes;
                Imports = imports;
                ToStringMapThreshold = toStringMapThreshold;
                FromStringMapThreshold = fromStringMapThreshold;

                _ValidateConfig();
            }

            private void _ValidateConfig()
            {
                // NOTE(v.matushkin): This should be warnings, not just some console message
                if (ToStringMapThreshold < 3)
                {
                    Console.WriteLine("There is no point in EnumGeneratorConfig.ToStringMapThreshold < 3, so setting its value to 3");
                }
                if (FromStringMapThreshold < 3)
                {
                    Console.WriteLine("There is no point in EnumGeneratorConfig.FromStringMapThreshold < 3, so setting its value to 3");
                }
            }
        }


        public readonly EnumGeneratorHppConfig Hpp;
        public readonly EnumGeneratorCppConfig Cpp;


        public EnumGeneratorConfig(EnumGeneratorHppConfig hpp, EnumGeneratorCppConfig cpp)
        {
            Hpp = hpp;
            Cpp = cpp;
        }
    }


    internal sealed class StructGeneratorConfig
    {
        public sealed class StructGeneratorHppConfig
        {
            public readonly string[] Imports;

            public StructGeneratorHppConfig(string[] imports)
            {
                Imports = imports;
            }
        }

        public sealed class StructGeneratorCppConfig
        {
            public readonly string[] Imports;

            public StructGeneratorCppConfig(string[] imports)
            {
                Imports = imports;
            }
        }


        public readonly StructGeneratorHppConfig Hpp;
        public readonly StructGeneratorCppConfig Cpp;


        public StructGeneratorConfig(StructGeneratorHppConfig hpp, StructGeneratorCppConfig cpp)
        {
            Hpp = hpp;
            Cpp = cpp;
        }
    }


    internal sealed class HesternalHeaderToolConfig
    {
        private const string k_ConfigFileName = "HesternalHeaderTool";


        public readonly string Namespace;
        public readonly string FilePathMacro;
        public readonly EnumGeneratorConfig EnumGenerator;
        public readonly StructGeneratorConfig StructGenerator;


        public HesternalHeaderToolConfig(
            string @namespace, string filePathMacro,
            EnumGeneratorConfig enumGenerator,
            StructGeneratorConfig structGenerator
            )
        {
            Namespace = @namespace;
            FilePathMacro = filePathMacro;
            EnumGenerator = enumGenerator;
            StructGenerator = structGenerator;
        }


        // NOTE(v.matushkin): And why did I choose to pass the config dir instead of the config file path?
        public static HesternalHeaderToolConfig LoadConfig(DirectoryInfo hesternalConfigDir)
        {
            var configFile = new FileInfo(Path.Combine(hesternalConfigDir.FullName, k_ConfigFileName + ".json"));
            if (configFile.Exists == false)
            {
                throw new ApplicationException(k_ConfigFileName + $" config doesn't exist: {configFile.FullName}");
            }

            var options = new JsonSerializerOptions
            {
                IncludeFields = true,
            };

            HesternalHeaderToolConfig config;
            using (FileStream fileStream = configFile.OpenRead())
            {
                config = JsonSerializer.Deserialize<HesternalHeaderToolConfig>(fileStream, options);
            }

            if (config == null)
            {
                throw new ApplicationException("Couldn't load " + k_ConfigFileName + " config: {configFile.FullName}");
            }

            return config;
        }
    }
}
