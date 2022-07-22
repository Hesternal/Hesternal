module;

#include <filesystem>
#include <string>

export module CopiumEngine.Core.Serialization.FileBinaryConverter;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Core.Platform.File;
import CopiumEngine.Core.Serialization.IBinaryConverter;


export namespace Copium
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

} // export namespace Copium
