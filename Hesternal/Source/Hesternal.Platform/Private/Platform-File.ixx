export module Hesternal.Platform:File;

import Hesternal.Core;

import <filesystem>;
import <fstream>;
import <string>;


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

        // NOTE(v.matushkin): Should I mark this as inline even when they are not defined here, probably not?
        [[nodiscard]] inline std::string ReadAllText(const std::string& filePath);
        [[nodiscard]] inline std::string ReadAllText(const std::filesystem::path& filePath);

    }; // namespace File

} // export namespace Hesternal
