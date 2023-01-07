export module HesternalEngine.Core.Serialization.FileBinaryConverter;

import Hesternal.Core;
import HesternalEngine.Core.Platform.File;
import HesternalEngine.Core.Serialization.IBinaryConverter;

import <filesystem>;
import <string>;


export namespace Hesternal
{

    class FileBinarySerializer final : public IBinaryConverter
    {
    public:
        FileBinarySerializer(const std::string& filePath);
        FileBinarySerializer(const std::filesystem::path& filePath);

        bool IsSerializing() const override { return true; };

        void Convert(void* data, int64 length) override;

    private:
        FileWriter m_fileWriter;
    };


    class FileBinaryDeserializer final : public IBinaryConverter
    {
    public:
        FileBinaryDeserializer(const std::string& filePath);
        FileBinaryDeserializer(const std::filesystem::path& filePath);

        bool IsSerializing() const override { return false; };

        void Convert(void* data, int64 length) override;

    private:
        FileReader m_fileReader;
    };

} // export namespace Hesternal
