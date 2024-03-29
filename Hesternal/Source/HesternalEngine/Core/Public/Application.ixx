export module HesternalEngine.Core.Application;

import Hesternal.Core;


export namespace Hesternal
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

        //< Platform dependent
        void PlatformInit();
        void PumpMessages();
        //> Platform dependent

        virtual void OnApplication_Init(int32 argc, const char* const* argv) = 0;
        virtual void OnApplication_Shutdown() = 0;
        virtual void OnApplication_Update() = 0;

    private:
        bool m_applicationInitialized;
        bool m_applicationClosed;
        bool m_shouldClose;
    };

} // export namespace Hesternal
