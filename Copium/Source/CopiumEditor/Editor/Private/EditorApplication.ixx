module;

// #include <filesystem>

export module CopiumEditor.EditorApplication;

import CopiumEngine.Application;
import CopiumEngine.Core.CoreTypes;


export namespace Copium
{

    class EditorApplication final : public Application
    {
    public:
        EditorApplication(int32 argc, const char* const* argv);
        ~EditorApplication() = default;

        // Application protected
        void InitSettings(EngineSettings& engineSettings) override;

    private:
        // void _CreateProject();
        // void _OpenProject();

    private:
        // std::filesystem::path m_projectPath;
    };

} // export namespace Copium
