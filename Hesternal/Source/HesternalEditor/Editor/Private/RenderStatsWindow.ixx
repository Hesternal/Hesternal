export module HesternalEditor.Editor.RenderStatsWindow;

import HesternalEditor.GUI.EditorWindow;


export namespace Hesternal
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

} // export namespace Hesternal
