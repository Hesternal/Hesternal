module;

#include <imgui.h>

export module CopiumEditor.Editor.TransformComponentGui;

import CopiumEngine.ECS.Components;

import CopiumEditor.GUI.ComponentGui;
import CopiumEditor.GUI.EditorGui;


export namespace Copium
{

    class TranslationGui : public ComponentGui
    {
    public:
        TranslationGui(Translation* translation)
            : ComponentGui("Translation")
            , m_translation(translation)
        {
        }

        ~TranslationGui() = default;

        TranslationGui(TranslationGui&& other) noexcept = default;
        TranslationGui& operator=(TranslationGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            EditorGui::VectorField(m_translation->Value);
        }

    private:
        Translation* m_translation;
    };


    class RotationGui : public ComponentGui
    {
    public:
        RotationGui(Rotation* rotation)
            : ComponentGui("Rotation")
            , m_rotation(rotation)
        {
        }

        ~RotationGui() = default;

        RotationGui(RotationGui&& other) noexcept = default;
        RotationGui& operator=(RotationGui&& other) noexcept = default;

    protected:
        void OnComponentGui()
        {
            EditorGui::QuaternionField(m_rotation->Value);
        }

    private:
        Rotation* m_rotation;
    };


    class ScaleGui : public ComponentGui
    {
    public:
        ScaleGui(Scale* scale)
            : ComponentGui("Scale")
            , m_scale(scale)
        {
        }

        ~ScaleGui() = default;

        ScaleGui(ScaleGui&& other) noexcept = default;
        ScaleGui& operator=(ScaleGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            EditorGui::FloatField(m_scale->Value);
        }

    private:
        Scale* m_scale;
    };


    class LocalToWorldGui : public ComponentGui
    {
    public:
        LocalToWorldGui(LocalToWorld* localToWorld)
            : ComponentGui("LocalToWorld")
            , m_localToWorld(localToWorld)
        {
        }

        ~LocalToWorldGui() = default;

        LocalToWorldGui(LocalToWorldGui&& other) noexcept = default;
        LocalToWorldGui& operator=(LocalToWorldGui&& other) noexcept = default;

    protected:
        void OnComponentGui() override
        {
            EditorGui::MatrixField(m_localToWorld->Value);
        }

    private:
        LocalToWorld* m_localToWorld;
    };


    class ParentGui : public ComponentGui
    {
    public:
        ParentGui(Parent* parent)
            : ComponentGui("Parent")
            , m_parent(parent)
        {
        }

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


    class ChildGui : public ComponentGui
    {
    public:
        ChildGui(Child* child)
            : ComponentGui("Child")
            , m_child(child)
        {
        }

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

} // export namespace Copium
