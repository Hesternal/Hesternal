export module HesternalEditor.GUI.EditorWindow;

import <string>;


export namespace Hesternal
{

    class EditorWindow
    {
    public:
        virtual ~EditorWindow() = default;

        EditorWindow(const EditorWindow& other) = delete;
        EditorWindow& operator=(const EditorWindow& other) = delete;
        EditorWindow(EditorWindow&& other) noexcept = default;
        EditorWindow& operator=(EditorWindow&& other) noexcept = default;

        void Draw();

    protected:
        EditorWindow(std::string&& windowTitle);

        virtual void OnGui() = 0;

    private:
        std::string m_windowTitle;
    };

} // export namespace Hesternal
