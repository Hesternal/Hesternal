module;

#include <filesystem>

export module CopiumEditor.EditorApplication;

import CopiumEngine.Application;
import CopiumEngine.Core.CoreTypes;


export namespace Copium
{

    class EditorApplication final : public Application
    {
    public:
        EditorApplication(int32 argc, const char* const* argv);
        ~EditorApplication();

        // Application protected
    private:
        void InitSettings(EngineSettings& engineSettings) override;
        void InitSystems() override;
        void ShutdownSystems() override;

    private:
        // void _CreateProject();
        // void _OpenProject();

    private:
        std::filesystem::path m_projectPath;
        bool                  m_editorInitialized;
        bool                  m_editorClosed;
    };

} // export namespace Copium
