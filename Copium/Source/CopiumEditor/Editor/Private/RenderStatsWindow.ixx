export module CopiumEditor.Editor.RenderStatsWindow;

import CopiumEditor.GUI.EditorWindow;


export namespace Copium
{

    class RenderStatsWindow final : public EditorWindow
    {
    public:
        RenderStatsWindow();
        ~RenderStatsWindow() = default;

        RenderStatsWindow(RenderStatsWindow&& other) noexcept = default;
        RenderStatsWindow& operator=(RenderStatsWindow&& other) noexcept = default;

    private:
        //< EditorWindow interface protected
        void OnGui() override;
        //> EditorWindow interface protected
    };

} // export namespace Copium
