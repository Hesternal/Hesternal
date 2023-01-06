export module HesternalEngine.Core.Serialization.IBinaryConverter;

import Hesternal.Core;

import HesternalEngine.Core.ForwardDeclaration;
import HesternalEngine.Core.Serialization.ObjectSerializationContext;

import <string>;
import <type_traits>;
import <unordered_map>;
import <utility>;
import <vector>;


// TODO(v.matushkin): Need type trait for CHT_STRUCT


export namespace Hesternal
{

    struct IBinaryConverter
    {
        virtual ~IBinaryConverter() = default;

        void SetObjectSerializationContext(ObjectSerializationContext* context) { m_objectSerializationContext = context; }

        virtual bool IsSerializing() const = 0;

        virtual void Convert(void* data, int64 length) = 0;

        IBinaryConverter& operator<<(bool& value)    { Convert(&value, sizeof(bool));    return *this; }
        IBinaryConverter& operator<<(char& value)    { Convert(&value, sizeof(char));    return *this; }
        IBinaryConverter& operator<<(int8& value)    { Convert(&value, sizeof(int8));    return *this; }
        IBinaryConverter& operator<<(int16& value)   { Convert(&value, sizeof(int16));   return *this; }
        IBinaryConverter& operator<<(int32& value)   { Convert(&value, sizeof(int32));   return *this; }
        IBinaryConverter& operator<<(int64& value)   { Convert(&value, sizeof(int64));   return *this; }
        IBinaryConverter& operator<<(uint8& value)   { Convert(&value, sizeof(uint8));   return *this; }
        IBinaryConverter& operator<<(uint16& value)  { Convert(&value, sizeof(uint16));  return *this; }
        IBinaryConverter& operator<<(uint32& value)  { Convert(&value, sizeof(uint32));  return *this; }
        IBinaryConverter& operator<<(uint64& value)  { Convert(&value, sizeof(uint64));  return *this; }
        IBinaryConverter& operator<<(float32& value) { Convert(&value, sizeof(float32)); return *this; }
        IBinaryConverter& operator<<(float64& value) { Convert(&value, sizeof(float64)); return *this; }

        IBinaryConverter& operator<<(Object& object);

        // NOTE(v.matushkin): No reason to make it for scoped enums only?
        template<typename TEnum> requires std::is_scoped_enum_v<TEnum>
        IBinaryConverter& operator<<(TEnum& value)
        {
            return *this << (std::underlying_type_t<TEnum>&)(value);
            // return *this << static_cast<std::underlying_type_t<TEnum>>(value);
            // return *this << &(std::to_underlying(value));
        }

        IBinaryConverter& operator<<(std::string& string)
        {
            if (IsSerializing())
            {
                int64 stringLength = string.length();
                *this << stringLength;

                if (stringLength != 0)
                {
                    Convert(string.data(), stringLength);
                }
            }
            else
            {
                int64 stringLength;
                *this << stringLength;

                if (stringLength != 0)
                {
                    string.resize_and_overwrite(stringLength, [this](char* buf, std::size_t buf_size) {
                        Convert(buf, buf_size);
                        return buf_size;
                    });
                }
                else
                {
                    string.clear();
                }
            }

            return *this;
        }

        // TODO(v.matushkin): Need to support more types
        // TODO(v.matushkin): Bring template requires back
        // NOTE(v.matushkin): No something like std::string::resize_and_overwrite for vector, which complicates things
        template<typename TElement> // requires std::is_fundamental_v<TElement> || std::is_trivial_v<TElement>
        IBinaryConverter& operator<<(std::vector<TElement>& vector)
        {
            if (IsSerializing())
            {
                int64 vectorLength = vector.size();
                *this << vectorLength;

                if (vectorLength != 0)
                {
                    if constexpr (std::is_fundamental_v<TElement>)
                    {
                        Convert(vector.data(), vectorLength);
                    }
                    else
                    {
                        for (int64 i = 0; i < vectorLength; i++)
                        {
                            *this << vector[i];
                        }
                    }
                }
            }
            else
            {
                int64 vectorLength;
                *this << vectorLength;

                if (vectorLength != 0)
                {
                    vector.reserve(vectorLength);
                    for (int64 i = 0; i < vectorLength; i++)
                    {
                        TElement element;
                        *this << element;
                        vector.push_back(std::move(element));
                    }
                }
                else
                {
                    vector.clear();
                }
            }

            return *this;
        }

        template<typename TKey, typename TValue>
        IBinaryConverter& operator<<(std::unordered_map<TKey, TValue>& map)
        {
            if (IsSerializing())
            {
                int64 mapLength = map.size();
                *this << mapLength;

                if (mapLength != 0)
                {
                    for (const auto& pair : map)
                    {
                        *this << pair.first;
                        *this << pair.second;
                    }
                }
            }
            else
            {
                int64 mapLength;
                *this << mapLength;

                if (mapLength != 0)
                {
                    map.reserve(mapLength);

                    for (int64 i = 0; i < mapLength; i++)
                    {
                        TKey key;
                        TValue value;
                        *this << key;
                        *this << value;
                        map.emplace(key, value);
                    }
                }
            }

            return *this;
        }

    private:
        ObjectSerializationContext* m_objectSerializationContext;
    };

} // export namespace Hesternal
