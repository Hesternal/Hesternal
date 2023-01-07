export module HesternalEditor.EditorApplication;

import Hesternal.Core;

import HesternalEngine.Engine.Application;
import HesternalEngine.Graphics.RenderGraph;
import HesternalEngine.Platform.Window;

import HesternalEditor.GUI.EditorWindow;

import <filesystem>;


export namespace Hesternal
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
        void OnEngine_Render(RenderContext& renderContext) override;
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

        std::unique_ptr<RenderGraph> m_renderGraph;

        std::vector<std::unique_ptr<EditorWindow>> m_editorWindows;
    };

} // export namespace Hesternal
