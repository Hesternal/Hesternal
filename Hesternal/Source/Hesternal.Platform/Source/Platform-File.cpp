module;

#include "Hesternal/Core/Debug.hpp"

module Hesternal.Platform;

import <sstream>;


// NOTE(v.matushkin): Error handling is very robust right now


namespace
{

    [[nodiscard]] static std::string ReadAllTextInternal(const std::ifstream& file)
    {
        HS_ASSERT(file.fail() == false);

        std::stringstream fileContent;
        fileContent << file.rdbuf();
        return fileContent.str();
    }

} // namespace


namespace Hesternal
{

    FileReader::FileReader(const std::string& filePath, bool binary /*= false*/)
        : m_fileInputStream(filePath, binary ? std::ios::binary : 0)
    {
        HS_ASSERT_MSG(m_fileInputStream.fail() == false, filePath);
    }

    FileReader::FileReader(const std::filesystem::path& filePath, bool binary /*= false*/)
        : m_fileInputStream(filePath, binary ? std::ios::binary : 0)
    {
        HS_ASSERT_MSG(m_fileInputStream.fail() == false, filePath.string());
    }

    void FileReader::Read(void* data, int64 length)
    {
        m_fileInputStream.read(reinterpret_cast<char*>(data), length);
    }


    FileWriter::FileWriter(const std::string& filePath, bool binary /*= false*/)
        : m_fileOutputStream(filePath, binary ? std::ios::binary : 0)
    {
        HS_ASSERT_MSG(m_fileOutputStream.fail() == false, filePath);
    }

    FileWriter::FileWriter(const std::filesystem::path& filePath, bool binary /*= false*/)
        : m_fileOutputStream(filePath, binary ? std::ios::binary : 0)
    {
        HS_ASSERT_MSG(m_fileOutputStream.fail() == false, filePath.string());
    }

    void FileWriter::Write(const void* data, int64 length)
    {
        m_fileOutputStream.write(reinterpret_cast<const char*>(data), length);
    }


    std::string File::ReadAllText(const std::string& filePath)
    {
        return ReadAllTextInternal(std::ifstream(filePath));
    }

    std::string File::ReadAllText(const std::filesystem::path& filePath)
    {
        return ReadAllTextInternal(std::ifstream(filePath));
    }

} // namespace Hesternal
