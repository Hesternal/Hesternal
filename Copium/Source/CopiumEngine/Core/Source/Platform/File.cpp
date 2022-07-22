module;

#include "Engine/Core/Debug.hpp"

#include <filesystem>
#include <fstream>

module CopiumEngine.Core.Platform.File;


// NOTE(v.matushkin): Error handling is very robust right now


namespace Copium
{

    FileReader::FileReader(const std::string& filePath, bool binary /*= false*/)
        : m_fileInputStream(filePath, binary ? std::ios::binary : 0)
    {
        COP_ASSERT_MSG(m_fileInputStream.fail() == false, filePath);
    }

    FileReader::FileReader(const std::filesystem::path& filePath, bool binary /*= false*/)
        : m_fileInputStream(filePath, binary ? std::ios::binary : 0)
    {
        COP_ASSERT_MSG(m_fileInputStream.fail() == false, filePath.string());
    }

    void FileReader::Read(void* data, int64 length)
    {
        m_fileInputStream.read(reinterpret_cast<char*>(data), length);
    }


    FileWriter::FileWriter(const std::string& filePath, bool binary /*= false*/)
        : m_fileOutputStream(filePath, binary ? std::ios::binary : 0)
    {
        COP_ASSERT_MSG(m_fileOutputStream.fail() == false, filePath);
    }

    FileWriter::FileWriter(const std::filesystem::path& filePath, bool binary /*= false*/)
        : m_fileOutputStream(filePath, binary ? std::ios::binary : 0)
    {
        COP_ASSERT_MSG(m_fileOutputStream.fail() == false, filePath.string());
    }

    void FileWriter::Write(const void* data, int64 length)
    {
        m_fileOutputStream.write(reinterpret_cast<const char*>(data), length);
    }

} // namespace Copium
