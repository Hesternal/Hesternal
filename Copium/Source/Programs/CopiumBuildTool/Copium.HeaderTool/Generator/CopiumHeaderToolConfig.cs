using System;
using System.IO;
using System.Text.Json;

namespace Copium.HeaderTool.Generator
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
        public sealed class StructGeneratorCppConfig
        {
            public readonly string[] Imports;

            public StructGeneratorCppConfig(string[] imports)
            {
                Imports = imports;
            }
        }


        public readonly StructGeneratorCppConfig Cpp;


        public StructGeneratorConfig(StructGeneratorCppConfig cpp)
        {
            Cpp = cpp;
        }
    }


    internal sealed class CopiumHeaderToolConfig
    {
        public readonly EnumGeneratorConfig EnumGenerator;
        public readonly StructGeneratorConfig StructGenerator;

        public CopiumHeaderToolConfig(
            EnumGeneratorConfig enumGenerator,
            StructGeneratorConfig structGenerator
            )
        {
            EnumGenerator = enumGenerator;
            StructGenerator = structGenerator;
        }


        public static CopiumHeaderToolConfig LoadConfig(DirectoryInfo copiumConfigDir)
        {
            var configFile = new FileInfo(Path.Combine(copiumConfigDir.FullName, "CopiumHeaderTool.json"));
            if (configFile.Exists == false)
            {
                throw new ApplicationException($"CopiumHeaderTool config doesn't exist: {configFile.FullName}");
            }

            var options = new JsonSerializerOptions
            {
                IncludeFields = true,
            };

            CopiumHeaderToolConfig config;
            using (FileStream fileStream = configFile.OpenRead())
            {
                config = JsonSerializer.Deserialize<CopiumHeaderToolConfig>(fileStream, options);
            }

            if (config == null)
            {
                throw new ApplicationException($"Couldn't load CopiumHeaderTool config: {configFile.FullName}");
            }

            return config;
        }
    }
}
