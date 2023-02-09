export module HesternalEditor.GUI.EditorGui;

import Hesternal.Core;
import Hesternal.Math;

import <vector>;
import <string_view>;


export namespace Hesternal
{

    class EditorGui final
    {
        enum class DataType : int32
        {
            S8,
            U8,
            S16,
            U16,
            S32,
            U32,
            S64,
            U64,
            Float,
            Double,
        };


        // TODO(v.matushkin): I should add an empty string as a global constant, just like C# string.Empty
        // TODO(v.matushkin): <FokinHeaderTool> can't parse empty string literal
        //   static inline const std::string_view k_EmptyLabel = "";
        static const std::string_view k_EmptyLabel;

    public:
        static void Indent();
        static void Unindent();


        //- Foldout

        [[nodiscard]] static bool BeginFoldoutGroup(std::string_view label);
        static void EndFoldoutGroup();


        //- Label

        static void Label(std::string_view label);
        static void Label(std::string_view label1, std::string_view label2);


        // NOTE(v.matushkin): I can technically combine all Field* functions with templates, but it will probably will be even uglier?

        //- Field

        //-- Scalar

        template<CArithmeticType T>
        [[maybe_unused]] static bool FieldScalar(T& value, T min = FundamentalType<T>::Min, T max = FundamentalType<T>::Max)
        {
            return FieldScalarInternal(k_EmptyLabel, GetDataType<T>(), static_cast<void*>(&value), static_cast<const void*>(&min), static_cast<const void*>(&max), GetPrintFormat<T>());
        }

        template<CArithmeticType T>
        [[maybe_unused]] static bool FieldScalar(std::string_view label, T& value, T min = FundamentalType<T>::Min, T max = FundamentalType<T>::Max)
        {
            return FieldScalarInternal(label, GetDataType<T>(), static_cast<void*>(&value), static_cast<const void*>(&min), static_cast<const void*>(&max), GetPrintFormat<T>());
        }

        //-- Vector

        [[maybe_unused]] static bool FieldVector(Float3& value);
        [[maybe_unused]] static bool FieldVector(std::string_view label, Float3& value);
        [[maybe_unused]] static bool FieldVector(Float4& value);
        [[maybe_unused]] static bool FieldVector(std::string_view label, Float4& value);

        //-- Matrix

        [[maybe_unused]] static bool FieldMatrix(Float4x4& value);

        //-- Quaternion

        [[maybe_unused]] static bool FieldQuaternion(Quaternion& value);
        [[maybe_unused]] static bool FieldQuaternion(std::string_view label, Quaternion& value);

        //-- Color

        [[maybe_unused]] static bool FieldColor(Color& value, bool showAlpha = true);
        [[maybe_unused]] static bool FieldColor(std::string_view label, Color& value, bool showAlpha = true);


        //- Slider

        template<CArithmeticType T>
        [[maybe_unused]] static bool SliderScalar(T& value, T min = FundamentalType<T>::Min, T max = FundamentalType<T>::Max)
        {
            return SliderScalarInternal(k_EmptyLabel, GetDataType<T>(), static_cast<void*>(&value), static_cast<const void*>(&min), static_cast<const void*>(&max));
        }

        template<CArithmeticType T>
        [[maybe_unused]] static bool SliderScalar(std::string_view label, T& value, T min = FundamentalType<T>::Min, T max = FundamentalType<T>::Max)
        {
            return SliderScalarInternal(label, GetDataType<T>(), static_cast<void*>(&value), static_cast<const void*>(&min), static_cast<const void*>(&max));
        }

    private:
        [[nodiscard]] static bool FieldScalarInternal(std::string_view label, DataType dataType, void* value, const void* min, const void* max, const char* format);
        [[nodiscard]] static bool FieldVectorInternal(std::string_view label, float32* value, int32 components);
        [[nodiscard]] static bool FieldColorInternal(std::string_view label, float32* value, bool showAlpha);

        [[nodiscard]] static bool SliderScalarInternal(std::string_view label, DataType dataType, void* value, const void* min, const void* max);


        template<CArithmeticType T>
        [[nodiscard]] static consteval DataType GetDataType() noexcept
        {
            if constexpr (std::is_same_v<T, int8>)    return DataType::S8;
            if constexpr (std::is_same_v<T, uint8>)   return DataType::U8;
            if constexpr (std::is_same_v<T, int16>)   return DataType::S16;
            if constexpr (std::is_same_v<T, uint16>)  return DataType::U16;
            if constexpr (std::is_same_v<T, int32>)   return DataType::S32;
            if constexpr (std::is_same_v<T, uint32>)  return DataType::U32;
            if constexpr (std::is_same_v<T, int64>)   return DataType::S64;
            if constexpr (std::is_same_v<T, uint64>)  return DataType::U64;
            if constexpr (std::is_same_v<T, float32>) return DataType::Float;
            if constexpr (std::is_same_v<T, float64>) return DataType::Double;
        }

        template<CArithmeticType T>
        [[nodiscard]] static consteval const char* GetPrintFormat() noexcept
        {
            // NOTE(v.matushkin): From ImGui
            //static const ImGuiDataTypeInfo GDataTypeInfo[] =
            //{
            //    { sizeof(char),             "S8",   "%d",   "%d"    },  // ImGuiDataType_S8
            //    { sizeof(unsigned char),    "U8",   "%u",   "%u"    },
            //    { sizeof(short),            "S16",  "%d",   "%d"    },  // ImGuiDataType_S16
            //    { sizeof(unsigned short),   "U16",  "%u",   "%u"    },
            //    { sizeof(int),              "S32",  "%d",   "%d"    },  // ImGuiDataType_S32
            //    { sizeof(unsigned int),     "U32",  "%u",   "%u"    },
            //#ifdef _MSC_VER
            //    { sizeof(ImS64),            "S64",  "%I64d","%I64d" },  // ImGuiDataType_S64
            //    { sizeof(ImU64),            "U64",  "%I64u","%I64u" },
            //#else
            //    { sizeof(ImS64),            "S64",  "%lld", "%lld"  },  // ImGuiDataType_S64
            //    { sizeof(ImU64),            "U64",  "%llu", "%llu"  },
            //#endif
            //    { sizeof(float),            "float", "%.3f","%f"    },  // ImGuiDataType_Float (float are promoted to double in va_arg)
            //    { sizeof(double),           "double","%f",  "%lf"   },  // ImGuiDataType_Double
            //};

            if constexpr (std::is_same_v<T, int8>)    return "%d";
            if constexpr (std::is_same_v<T, uint8>)   return "%u";
            if constexpr (std::is_same_v<T, int16>)   return "%d";
            if constexpr (std::is_same_v<T, uint16>)  return "%u";
            if constexpr (std::is_same_v<T, int32>)   return "%d";
            if constexpr (std::is_same_v<T, uint32>)  return "%u";
            if constexpr (std::is_same_v<T, int64>)   return "%lld";
            if constexpr (std::is_same_v<T, uint64>)  return "%llu";
            if constexpr (std::is_same_v<T, float32>) return "%.3f";
            if constexpr (std::is_same_v<T, float64>) return "%lf";
        }

    private:
        static inline std::vector<int32> s_idStack = { 0 };
    };


    bool EditorGui::FieldVector(Float3& value)
    {
        return FieldVectorInternal(k_EmptyLabel, reinterpret_cast<float32*>(&value), 3);
    }

    bool EditorGui::FieldVector(std::string_view label, Float3& value)
    {
        return FieldVectorInternal(label, reinterpret_cast<float32*>(&value), 3);
    }

    bool EditorGui::FieldVector(Float4& value)
    {
        return FieldVectorInternal(k_EmptyLabel, reinterpret_cast<float32*>(&value), 4);
    }

    bool EditorGui::FieldVector(std::string_view label, Float4& value)
    {
        return FieldVectorInternal(label, reinterpret_cast<float32*>(&value), 4);
    }

    bool EditorGui::FieldQuaternion(Quaternion& value)
    {
        return FieldQuaternion(k_EmptyLabel, value);
    }

    // TODO(v.matushkin): <FokinHeaderTool> can't parse multiline comments -> bool showAlpha /*= true */
    bool EditorGui::FieldColor(Color& value, bool showAlpha)
    {
        return FieldColorInternal(k_EmptyLabel, reinterpret_cast<float32*>(&value), showAlpha);
    }

    // TODO(v.matushkin): <FokinHeaderTool> can't parse multiline comments -> bool showAlpha /*= true */
    bool EditorGui::FieldColor(std::string_view label, Color& value, bool showAlpha)
    {
        return FieldColorInternal(label, reinterpret_cast<float32*>(&value), showAlpha);
    }


#define INSTANTIATE_FUNCTIONS(type) \
    template bool EditorGui::FieldScalar<type>(type& value, type min, type max); \
    template bool EditorGui::FieldScalar<type>(std::string_view label, type& value, type min, type max); \
    template bool EditorGui::SliderScalar<type>(type& value, type min, type max); \
    template bool EditorGui::SliderScalar<type>(std::string_view label, type& value, type min, type max);

    INSTANTIATE_FUNCTIONS(int8);
    INSTANTIATE_FUNCTIONS(int16);
    INSTANTIATE_FUNCTIONS(int32);
    INSTANTIATE_FUNCTIONS(int64);
    INSTANTIATE_FUNCTIONS(uint8);
    INSTANTIATE_FUNCTIONS(uint16);
    INSTANTIATE_FUNCTIONS(uint32);
    INSTANTIATE_FUNCTIONS(uint64);
    INSTANTIATE_FUNCTIONS(float32);
    INSTANTIATE_FUNCTIONS(float64);

#undef INSTANTIATE_FUNCTIONS

} // export namespace Hesternal
