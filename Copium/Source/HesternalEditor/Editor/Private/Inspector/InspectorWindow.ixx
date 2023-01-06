export module HesternalEditor.Editor.InspectorWindow;

import HesternalEditor.GUI.EditorWindow;


export namespace Hesternal
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

} // export namespace Hesternal
