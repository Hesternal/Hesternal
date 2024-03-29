module;

#include <imgui.h>

export module HesternalEditor.Editor.TransformComponentGui;

import HesternalEngine.ECS.Components;

import HesternalEditor.GUI.ComponentGui;
import HesternalEditor.GUI.EditorGui;


export namespace Hesternal
{

    class TransformGui final : public ComponentGui
    {
    public:
        TransformGui(Transform* transform)
            : ComponentGui("Transform")
            , m_transform(transform)
        {}

        ~TransformGui() = default;

        TransformGui(TransformGui&& other) noexcept = default;
        TransformGui& operator=(TransformGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            EditorGui::FieldVector("Position", m_transform->Position);
            EditorGui::FieldQuaternion("Rotation", m_transform->Rotation);
            EditorGui::FieldScalar("Scale", m_transform->Scale);
        }

    private:
        Transform* m_transform;
    };


    class LocalToWorldGui final : public ComponentGui
    {
    public:
        LocalToWorldGui(LocalToWorld* localToWorld)
            : ComponentGui("LocalToWorld")
            , m_localToWorld(localToWorld)
        {}

        ~LocalToWorldGui() = default;

        LocalToWorldGui(LocalToWorldGui&& other) noexcept = default;
        LocalToWorldGui& operator=(LocalToWorldGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            EditorGui::FieldMatrix(m_localToWorld->Value);
        }

    private:
        LocalToWorld* m_localToWorld;
    };


    class ParentGui final : public ComponentGui
    {
    public:
        ParentGui(Parent* parent)
            : ComponentGui("Parent")
            , m_parent(parent)
        {}

        ~ParentGui() = default;

        ParentGui(ParentGui&& other) noexcept = default;
        ParentGui& operator=(ParentGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            ImGui::Text("Entity {%d}", m_parent->Value);
        }

    private:
        Parent* m_parent;
    };


    class ChildGui final : public ComponentGui
    {
    public:
        ChildGui(Child* child)
            : ComponentGui("Child")
            , m_child(child)
        {}

        ~ChildGui() = default;

        ChildGui(ChildGui&& other) noexcept = default;
        ChildGui& operator=(ChildGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            ImGui::Text("Child count: {%d}", m_child->Children.size());
        }

    private:
        Child* m_child;
    };

} // export namespace Hesternal
