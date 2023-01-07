export module HesternalEditor.Editor.HierarchyWindow;

import HesternalEditor.GUI.EditorWindow;


export namespace Hesternal
{

    class HierarchyWindow final : public EditorWindow
    {
    public:
        HierarchyWindow();
        ~HierarchyWindow() = default;

        HierarchyWindow(HierarchyWindow&& other) noexcept = default;
        HierarchyWindow& operator=(HierarchyWindow&& other) noexcept = default;

    private:
        //< EditorWindow interface protected
        void OnGui() override;
        //> EditorWindow interface protected
    };

} // export namespace Hesternal
