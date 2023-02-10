module HesternalEngine.Core.Serialization.FileBinaryConverter;

import Hesternal.Platform;

import <filesystem>;
import <string>;


namespace Hesternal
{

    FileBinarySerializer::FileBinarySerializer(const std::string& filePath)
        : m_fileWriter(filePath, true)
    {
    }

    FileBinarySerializer::FileBinarySerializer(const std::filesystem::path& filePath)
        : m_fileWriter(filePath, true)
    {
    }

    void FileBinarySerializer::Convert(void* data, int64 length)
    {
        m_fileWriter.Write(data, length);
    }


    FileBinaryDeserializer::FileBinaryDeserializer(const std::string& filePath)
        : m_fileReader(filePath, true)
    {
    }

    FileBinaryDeserializer::FileBinaryDeserializer(const std::filesystem::path& filePath)
        : m_fileReader(filePath, true)
    {
    }

    void FileBinaryDeserializer::Convert(void* data, int64 length)
    {
        m_fileReader.Read(data, length);
    }

} // namespace Hesternal
