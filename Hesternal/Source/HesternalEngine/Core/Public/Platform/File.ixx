module;

//#include <fstream>

export module HesternalEngine.Core.Platform.File;

import Hesternal.Core;

// NOTE(v.matushkin): Waiting until https://github.com/microsoft/STL/issues/3112 gets fixed
//  Although seems like it's working without this headers anyway?
//import <filesystem>;
//import <string>;
import <fstream>;


export namespace Hesternal
{

    class FileReader final
    {
    public:
        FileReader(const std::string& filePath, bool binary = false);
        FileReader(const std::filesystem::path& filePath, bool binary = false);

        void Read(void* data, int64 length);

    private:
        std::ifstream m_fileInputStream;
    };


    class FileWriter final
    {
    public:
        FileWriter(const std::string& filePath, bool binary = false);
        FileWriter(const std::filesystem::path& filePath, bool binary = false);

        void Write(const void* data, int64 length);

    private:
        std::ofstream m_fileOutputStream;
    };


    namespace File
    {

        [[nodiscard]] inline std::string ReadAllText(const std::string& filePath);
        [[nodiscard]] inline std::string ReadAllText(const std::filesystem::path& filePath);

    }; // namespace File

} // export namespace Hesternal
