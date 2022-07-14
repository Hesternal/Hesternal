module;

#include <filesystem>
#include <fstream>

export module CopiumEngine.Core.Platform.File;

import CopiumEngine.Core.CoreTypes;


export namespace Copium
{

    class FileReader final
    {
    public:
        FileReader(const std::filesystem::path& filePath, bool binary = false);

        void Read(void* data, int64 length);

    private:
        std::ifstream m_fileInputStream;
    };


    class FileWriter final
    {
    public:
        FileWriter(const std::filesystem::path& filePath, bool binary = false);

        void Write(const void* data, int64 length);

    private:
        std::ofstream m_fileOutputStream;
    };

} // export namespace Copium
