export module CopiumEditor.GUI.ComponentGui;

import CopiumEditor.GUI.EditorGui;


export namespace Copium
{

    class ComponentGui
    {
    public:
        virtual ~ComponentGui() = default;

        ComponentGui(const ComponentGui& other) = delete;
        ComponentGui& operator=(const ComponentGui& other) = delete;
        ComponentGui(ComponentGui&& other) noexcept = default;
        ComponentGui& operator=(ComponentGui&& other) noexcept = default;

        void Draw()
        {
            if (EditorGui::BeginFoldoutGroup(m_componentName))
            {
                OnComponentGui();
                EditorGui::EndFoldoutGroup();
            }
        }

    protected:
        ComponentGui(const char* componentName)
            : m_componentName(componentName)
        {
        }

        virtual void OnComponentGui() = 0;

    private:
        const char* m_componentName;
    };

} // export namespace Copium
