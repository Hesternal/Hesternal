export module HesternalEditor.GUI.EditorGui;

import Hesternal.Core;
import Hesternal.Math;

import <vector>;
import <string_view>;


export namespace Hesternal
{

    class EditorGui final
    {
        // TODO(v.matushkin): I should add an empty string as a global constant, just like C# string.Empty
        // TODO(v.matushkin): <FokinHeaderTool> can't parse empty string literal
        //   static inline const std::string_view k_EmptyLabel = "";
        static const std::string_view k_EmptyLabel;

    public:
        static void Indent();
        static void Unindent();

        [[nodiscard]] static bool BeginFoldoutGroup(std::string_view label);
        static void EndFoldoutGroup();

        static void Label(std::string_view label);
        static void Label(std::string_view label1, std::string_view label2);

        [[maybe_unused]] static bool FloatField(float32& value);
        [[maybe_unused]] static bool FloatField(std::string_view label, float32& value);
        [[maybe_unused]] static bool VectorField(Float3& value);
        [[maybe_unused]] static bool VectorField(std::string_view label, Float3& value);
        [[maybe_unused]] static bool VectorField(Float4& value);
        [[maybe_unused]] static bool VectorField(std::string_view label, Float4& value);
        [[maybe_unused]] static bool MatrixField(Float4x4& value);
        [[maybe_unused]] static bool QuaternionField(Quaternion& value);
        [[maybe_unused]] static bool QuaternionField(std::string_view label, Quaternion& value);

        [[maybe_unused]] static bool ColorField(Color& value, bool showAlpha = true);
        [[maybe_unused]] static bool ColorField(std::string_view label, Color& value, bool showAlpha = true);

    private:
        [[maybe_unused]] static bool DragScalar(std::string_view label, float32& value);
        [[maybe_unused]] static bool DragVector(std::string_view label, float32* value, int32 components);

        [[maybe_unused]] static bool EditColor(std::string_view label, float32* value, bool showAlpha);

    private:
        static inline std::vector<int32> s_idStack = { 0 };
    };


    bool EditorGui::FloatField(float32& value)
    {
        return DragScalar(k_EmptyLabel, value);
    }

    bool EditorGui::FloatField(std::string_view label, float32& value)
    {
        return DragScalar(label, value);
    }

    bool EditorGui::VectorField(Float3& value)
    {
        return DragVector(k_EmptyLabel, reinterpret_cast<float32*>(&value), 3);
    }

    bool EditorGui::VectorField(std::string_view label, Float3& value)
    {
        return DragVector(label, reinterpret_cast<float32*>(&value), 3);
    }

    bool EditorGui::VectorField(Float4& value)
    {
        return DragVector(k_EmptyLabel, reinterpret_cast<float32*>(&value), 4);
    }

    bool EditorGui::VectorField(std::string_view label, Float4& value)
    {
        return DragVector(label, reinterpret_cast<float32*>(&value), 4);
    }

    bool EditorGui::QuaternionField(Quaternion& value)
    {
        return QuaternionField(k_EmptyLabel, value);
    }


    // TODO(v.matushkin): <FokinHeaderTool> can't parse multiline comments -> bool showAlpha /*= true */
    bool EditorGui::ColorField(Color& value, bool showAlpha)
    {
        return EditColor(k_EmptyLabel, reinterpret_cast<float32*>(&value), showAlpha);
    }

    // TODO(v.matushkin): <FokinHeaderTool> can't parse multiline comments -> bool showAlpha /*= true */
    bool EditorGui::ColorField(std::string_view label, Color& value, bool showAlpha)
    {
        return EditColor(label, reinterpret_cast<float32*>(&value), showAlpha);
    }

} // export namespace Hesternal
