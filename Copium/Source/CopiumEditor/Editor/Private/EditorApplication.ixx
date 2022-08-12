export module CopiumEditor.EditorApplication;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Engine.Application;
import CopiumEngine.Platform.Window;

import <filesystem>;


export namespace Copium
{

    class EditorApplication final : public EngineApplication
    {
    public:
        EditorApplication();
        ~EditorApplication();

    private:
        //< EngineApplication Interface Protected
        void OnEngine_Init(int32 argc, const char* const* argv) override;
        void OnEngine_SystemsInit() override;
        void OnEngine_Shutdown() override;
        void OnEngine_Update() override;
        //> EngineApplication Interface Protected

    private:
        // void _CreateProject();
        // void _OpenProject();

    private:
        std::filesystem::path   m_projectPath;
        std::filesystem::path   m_shaderDirPath;
        std::unique_ptr<Window> m_mainWindow;
        bool                    m_editorInitialized;
        bool                    m_editorClosed;
    };

} // export namespace Copium
