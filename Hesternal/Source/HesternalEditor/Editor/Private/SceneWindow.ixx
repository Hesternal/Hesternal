export module HesternalEditor.Editor.SceneWindow;

import HesternalEngine.Graphics.GraphicsTypes;

import HesternalEditor.GUI.EditorWindow;


export namespace Hesternal
{

    class SceneWindow final : public EditorWindow
    {
    public:
        SceneWindow(RenderTextureHandle engineOutputHandle);
        ~SceneWindow() = default;

        SceneWindow(SceneWindow&& other) noexcept = default;
        SceneWindow& operator=(SceneWindow&& other) noexcept = default;

    private:
        //< EditorWindow interface protected
        void OnGui() override;
        //> EditorWindow interface protected

    private:
        RenderTextureHandle m_engineOutputHandle;
    };

} // export namespace Hesternal
