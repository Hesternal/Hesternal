export module CopiumEngine.Engine.Application;

import CopiumEngine.Core.Application;
import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Engine.EngineSettings;
import CopiumEngine.Platform.Window;

import <memory>;


export namespace Copium
{

    class EngineApplication : public Application
    {
    public:
        // NOTE(v.matushkin): Questionable method
        [[nodiscard]] Window* GetMainWindow() const { return m_mainWindow.get(); }

    protected:
        EngineApplication();
        ~EngineApplication();

        // NOTE(v.matushkin): Need separate Init and SystemsInit cause Editor sets EngineSettings,
        //  probably this can be reworked somehow, but right now idk how.
        virtual void OnEngine_Init(int32 argc, const char* const* argv, EngineSettings& engineSettings) = 0;
        virtual void OnEngine_SystemsInit() = 0;
        virtual void OnEngine_Shutdown() = 0;
        virtual void OnEngine_Update() = 0;

    private:
        //< Aplication Interface Protected
        void OnApplication_Init(int32 argc, const char* const* argv) override;
        void OnApplication_Shutdown() override;
        void OnApplication_Update() override;
        //> Aplication Interface Protected

        void _SystemsInit();
        void _SystemsShutdown();

    private:
        EngineSettings          m_engineSettings;
        std::unique_ptr<Window> m_mainWindow;
        bool                    m_engineInitialized;
        bool                    m_engineClosed;
    };

} // export namespace Copium
