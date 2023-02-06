module;

#include <imgui.h>

module HesternalEditor.GUI.EditorGui;


const std::string_view Hesternal::EditorGui::k_EmptyLabel = "";


namespace
{
    using namespace Hesternal;


    static constexpr float32 k_Indent         = 10.0f;
    static constexpr float32 k_FloatDragSpeed = 0.05f;

} // namespace


namespace Hesternal
{

    void EditorGui::Indent()
    {
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


    bool EditorGui::MatrixField(Float4x4& value)
    {
        ImGui::PushID(s_idStack.back()++);

        bool changed = false;
        changed |= VectorField(value.C0);
        changed |= VectorField(value.C1);
        changed |= VectorField(value.C2);
        changed |= VectorField(value.C3);

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

    bool EditorGui::QuaternionField(std::string_view label, Quaternion& value)
    {
        ImGui::PushID(s_idStack.back()++);

        Float4 asVector(value.X, value.Y, value.Z, value.W);
        const bool changed = VectorField(label, asVector);

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


    bool EditorGui::DragScalar(std::string_view label, float32& value)
    {
        ImGui::PushID(s_idStack.back()++);
        const bool changed = ImGui::DragScalar(label.data(), ImGuiDataType_Float, &value, k_FloatDragSpeed);
        ImGui::PopID();
        return changed;
    }

    bool EditorGui::DragVector(std::string_view label, float32* value, int32 components)
    {
        ImGui::PushID(s_idStack.back()++);
        const bool changed = ImGui::DragScalarN(label.data(), ImGuiDataType_Float, value, components, k_FloatDragSpeed);
        ImGui::PopID();
        return changed;
    }


    bool EditorGui::EditColor(std::string_view label, float32* value, bool showAlpha)
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

} // namespace Hesternal
