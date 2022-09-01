export module CopiumEditor.Editor.SceneWindow;

import CopiumEngine.Graphics.GraphicsTypes;

import CopiumEditor.GUI.EditorWindow;


export namespace Copium
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

} // export namespace Copium
