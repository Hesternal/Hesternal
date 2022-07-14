module;

#include <string>
#include <type_traits>
#include <utility>

export module CopiumEngine.Core.Serialization.IBinaryConverter;

import CopiumEngine.Core.CoreTypes;


export namespace Copium
{

    struct IBinaryConverter
    {
        virtual ~IBinaryConverter() = default;

        virtual bool IsSerializing() const = 0;

        virtual void Convert(void* data, int64 length) = 0;

        IBinaryConverter& operator<<(bool& value)    { Convert(&value, 1); return *this; }
        IBinaryConverter& operator<<(char& value)    { Convert(&value, 1); return *this; }
        IBinaryConverter& operator<<(int8& value)    { Convert(&value, 1); return *this; }
        IBinaryConverter& operator<<(int16& value)   { Convert(&value, 2); return *this; }
        IBinaryConverter& operator<<(int32& value)   { Convert(&value, 4); return *this; }
        IBinaryConverter& operator<<(int64& value)   { Convert(&value, 8); return *this; }
        IBinaryConverter& operator<<(uint8& value)   { Convert(&value, 1); return *this; }
        IBinaryConverter& operator<<(uint16& value)  { Convert(&value, 2); return *this; }
        IBinaryConverter& operator<<(uint32& value)  { Convert(&value, 4); return *this; }
        IBinaryConverter& operator<<(uint64& value)  { Convert(&value, 8); return *this; }
        IBinaryConverter& operator<<(float32& value) { Convert(&value, 4); return *this; }
        IBinaryConverter& operator<<(float64& value) { Convert(&value, 8); return *this; }

        // NOTE(v.matushkin): No reason to make it for scoped enums only?
        template<typename TEnum> requires std::is_scoped_enum_v<TEnum>
        IBinaryConverter& operator<<(TEnum& value)
        {
            return *this << (std::underlying_type_t<TEnum>&)(value);
            // return *this << static_cast<std::underlying_type_t<TEnum>>(value);
            // return *this << &(std::to_underlying(value));
        }

        IBinaryConverter& operator<<(std::string& value)
        {
            int64 stringLength = value.length();
            *this << stringLength;

            if (IsSerializing())
            {
                Convert(value.data(), stringLength);
            }
            else
            {
                value.resize_and_overwrite(stringLength, [this](char* buf, std::size_t buf_size) {
                    Convert(buf, buf_size);
                    return buf_size;
                });
            }

            return *this;
        }
    };

} // export namespace Copium
