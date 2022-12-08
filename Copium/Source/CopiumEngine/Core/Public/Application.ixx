export module CopiumEngine.Core.Application;

import Copium.Core;


export namespace Copium
{

    class Application
    {
    public:
        [[nodiscard]] static Application* Get();


        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&& other) = delete;
        Application& operator=(Application&& other) = delete;

        // TODO(v.matushkin): Init/Shutdown/Run should be accessible only by main() ?
        void Init(int32 argc, const char* const* argv);
        void Shutdown();

        void Run();

    protected:
        Application();
        ~Application();

        void Close() { m_shouldClose = true; }

        //< Platform dependant
        void PlatformInit();
        void PumpMessages();
        //> Platform dependant

        virtual void OnApplication_Init(int32 argc, const char* const* argv) = 0;
        virtual void OnApplication_Shutdown() = 0;
        virtual void OnApplication_Update() = 0;

    private:
        bool m_applicationInitialized;
        bool m_applicationClosed;
        bool m_shouldClose;
    };

} // export namespace Copium
