export module CopiumEditor.GUI.EditorGui;

import Copium.Core;
import Copium.Math;

import <vector>;
import <string_view>;


export namespace Copium
{

    class EditorGui final
    {
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

    private:
        [[maybe_unused]] static bool DragScalar(float32& value);
        [[maybe_unused]] static bool DragScalar(std::string_view label, float32& value);
        [[maybe_unused]] static bool DragVector(float32* value, int32 components);
        [[maybe_unused]] static bool DragVector(std::string_view label, float32* value, int32 components);

    private:
        static inline std::vector<int32> s_idStack = { 0 };
    };


    bool EditorGui::FloatField(float32& value)
    {
        return DragScalar(value);
    }

    bool EditorGui::FloatField(std::string_view label, float32& value)
    {
        return DragScalar(label, value);
    }

    bool EditorGui::VectorField(Float3& value)
    {
        return DragVector(reinterpret_cast<float32*>(&value), 3);
    }

    bool EditorGui::VectorField(std::string_view label, Float3& value)
    {
        return DragVector(label, reinterpret_cast<float32*>(&value), 3);
    }

    bool EditorGui::VectorField(Float4& value)
    {
        return DragVector(reinterpret_cast<float32*>(&value), 4);
    }

    bool EditorGui::VectorField(std::string_view label, Float4& value)
    {
        return DragVector(label, reinterpret_cast<float32*>(&value), 4);
    }

} // export namespace Copium
