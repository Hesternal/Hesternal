module;

#include <memory>

export module CopiumEngine.Application;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Engine.EngineSettings;
import CopiumEngine.Graphics;
import CopiumEngine.Platform.Window;


export namespace Copium
{

    class Application
    {
    public:
        Application();
        virtual ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&& other) noexcept = default;
        Application& operator=(Application&& other) noexcept = default;

        void Run();

    protected:
        virtual void InitSettings(EngineSettings& engineSettings) = 0;

    private:
        void _EngineInit();
        void _EngineLoop();

    private:
        EngineSettings          m_engineSettings;

        IGraphicsDevice*        m_graphicsDevice;
        std::unique_ptr<Window> m_mainWindow;
        ShaderHandle            m_triangleShaderHandle;
    };


    extern std::unique_ptr<Application> CreateApplication(int32 argc, const char* const* argv);

} // export namespace Copium
