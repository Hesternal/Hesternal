export module CopiumEditor.GUI.Selection;

import Copium.ECS;


// NOTE(v.matushkin): Not sure that this class should be in the GUI Module


export namespace Copium
{

    class Selection final
    {
    public:
        [[nodiscard]] static Entity GetSelectedEntity() noexcept { return m_selectedEntity; }

        [[nodiscard]] static bool IsSelected(Entity entity) noexcept { return m_selectedEntity == entity; }

        static void SetSelection(Entity entity) noexcept { m_selectedEntity = entity; }

    private:
        static inline Entity m_selectedEntity = Entity::Invalid;
    };

} // export namespace Copium
