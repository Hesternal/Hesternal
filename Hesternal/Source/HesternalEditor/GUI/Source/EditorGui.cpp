module;

#include <imgui.h>

module HesternalEditor.GUI.EditorGui;


const std::string_view Hesternal::EditorGui::k_EmptyLabel = "";


namespace
{
    using namespace Hesternal;


    static constexpr float32 k_Indent         = 10.0f;
    static constexpr float32 k_FloatDragSpeed = 0.05f;

    static constexpr ImGuiSliderFlags_ k_DragScalarFlags = ImGuiSliderFlags_AlwaysClamp;
    static constexpr ImGuiSliderFlags_ k_SliderScalarFlags = ImGuiSliderFlags_AlwaysClamp;

} // namespace


namespace Hesternal
{

    void EditorGui::Indent()
    {
        // NOTE(v.matushkin): Never done shit like this, but where else can I put this?
        static_assert(sizeof(DataType) == sizeof(ImGuiDataType_));
        static_assert(static_cast<int32>(DataType::S8)     == ImGuiDataType_S8);
        static_assert(static_cast<int32>(DataType::U8)     == ImGuiDataType_U8);
        static_assert(static_cast<int32>(DataType::S16)    == ImGuiDataType_S16);
        static_assert(static_cast<int32>(DataType::U16)    == ImGuiDataType_U16);
        static_assert(static_cast<int32>(DataType::S32)    == ImGuiDataType_S32);
        static_assert(static_cast<int32>(DataType::U32)    == ImGuiDataType_U32);
        static_assert(static_cast<int32>(DataType::S64)    == ImGuiDataType_S64);
        static_assert(static_cast<int32>(DataType::U64)    == ImGuiDataType_U64);
        static_assert(static_cast<int32>(DataType::Float)  == ImGuiDataType_Float);
        static_assert(static_cast<int32>(DataType::Double) == ImGuiDataType_Double);


        ImGui::Indent(k_Indent);
    }

    void EditorGui::Unindent()
    {
        ImGui::Indent(-k_Indent);
    }


    // NOTE(v.matushkin): Same as ImGui::CollapsingHeader() without ImGuiTreeNodeFlags_NoTreePushOnOpen flag
    bool EditorGui::BeginFoldoutGroup(std::string_view label)
    {
        const bool isOpened = ImGui::TreeNodeEx(label.data(), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_DefaultOpen);
        if (isOpened)
        {
            s_idStack.push_back(0);
        }
        return isOpened;
    }

    void EditorGui::EndFoldoutGroup()
    {
        s_idStack.pop_back();
        ImGui::TreePop();
    }


    void EditorGui::Label(std::string_view label)
    {
        ImGui::PushID(s_idStack.back()++);
        ImGui::TextUnformatted(&*label.cbegin());
        ImGui::PopID();
    }

    // TODO(v.matushkin): Look at how ImGui::LabelText works (aligning, clamping)
    void EditorGui::Label(std::string_view label1, std::string_view label2)
    {
        ImGui::PushID(s_idStack.back()++);
        ImGui::TextUnformatted(&*label1.cbegin());
        ImGui::SameLine();
        ImGui::TextUnformatted(&*label2.cbegin());
        ImGui::PopID();
    }


    bool EditorGui::FieldMatrix(Float4x4& value)
    {
        ImGui::PushID(s_idStack.back()++);

        bool changed = false;
        changed |= FieldVector(value.C0);
        changed |= FieldVector(value.C1);
        changed |= FieldVector(value.C2);
        changed |= FieldVector(value.C3);

        //const float32 itemWidth = ImGui::CalcItemWidth() * 0.25f;
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("X", &value.C0.X); ImGui::SameLine();
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("Y", &value.C1.X); ImGui::SameLine();
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("Z", &value.C2.X); ImGui::SameLine();
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("W", &value.C3.X);
        //
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("X", &value.C0.Y); ImGui::SameLine();
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("Y", &value.C1.Y); ImGui::SameLine();
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("Z", &value.C2.Y); ImGui::SameLine();
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("W", &value.C3.Y);
        //
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("X", &value.C0.Z); ImGui::SameLine();
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("Y", &value.C1.Z); ImGui::SameLine();
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("Z", &value.C2.Z); ImGui::SameLine();
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("W", &value.C3.Z);
        //
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("X", &value.C0.W); ImGui::SameLine();
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("Y", &value.C1.W); ImGui::SameLine();
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("Z", &value.C2.W); ImGui::SameLine();
        //ImGui::SetNextItemWidth(itemWidth); changed |= ImGui::InputFloat("W", &value.C3.W);

        ImGui::PopID();

        return changed;
    }

    bool EditorGui::FieldQuaternion(std::string_view label, Quaternion& value)
    {
        ImGui::PushID(s_idStack.back()++);

        Float4 asVector(value.X, value.Y, value.Z, value.W);
        const bool changed = FieldVector(label, asVector);

        if (changed)
        {
            value.X = asVector.X;
            value.Y = asVector.Y;
            value.Z = asVector.Z;
            value.W = asVector.W;
        }

        ImGui::PopID();

        return changed;
    }


    bool EditorGui::FieldScalarInternal(std::string_view label, DataType dataType, void* value, const void* min, const void* max, const char* format)
    {
        ImGui::PushID(s_idStack.back()++);
        const bool changed = ImGui::DragScalar(label.data(), static_cast<ImGuiDataType_>(dataType), value, k_FloatDragSpeed, min, max, format, k_DragScalarFlags);
        ImGui::PopID();
        return changed;
    }

    bool EditorGui::FieldVectorInternal(std::string_view label, float32* value, int32 components)
    {
        ImGui::PushID(s_idStack.back()++);
        const bool changed = ImGui::DragScalarN(label.data(), ImGuiDataType_Float, value, components, k_FloatDragSpeed);
        ImGui::PopID();
        return changed;
    }

    bool EditorGui::FieldColorInternal(std::string_view label, float32* value, bool showAlpha)
    {
        int32 imguiColorFlags = ImGuiColorEditFlags_Float;
        if (showAlpha == false)
        {
            imguiColorFlags |= ImGuiColorEditFlags_NoAlpha;
        }

        ImGui::PushID(s_idStack.back()++);
        const bool changed = ImGui::ColorEdit4(label.data(), value, imguiColorFlags);
        ImGui::PopID();
        return changed;
    }


    bool EditorGui::SliderScalarInternal(std::string_view label, DataType dataType, void* value, const void* min, const void* max)
    {
        ImGui::PushID(s_idStack.back()++);
        const bool changed = ImGui::SliderScalar(label.data(), static_cast<ImGuiDataType_>(dataType), value, min, max, nullptr, k_SliderScalarFlags);
        ImGui::PopID();
        return changed;
    }

} // namespace Hesternal
