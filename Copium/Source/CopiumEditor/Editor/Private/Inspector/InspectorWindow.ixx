export module CopiumEditor.Editor.InspectorWindow;

import CopiumEditor.GUI.EditorWindow;


export namespace Copium
{

    class InspectorWindow final : public EditorWindow
    {
    public:
        InspectorWindow();
        ~InspectorWindow() = default;

        InspectorWindow(InspectorWindow&& other) noexcept = default;
        InspectorWindow& operator=(InspectorWindow&& other) noexcept = default;

    private:
        //< EditorWindow interface protected
        void OnGui() override;
        //> EditorWindow interface protected
    };

} // export namespace Copium
