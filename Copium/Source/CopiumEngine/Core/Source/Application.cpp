module;

#include "Engine/Core/Debug.hpp"

module CopiumEngine.Core.Application;


namespace Copium
{

    Application::Application()
        : m_applicationInitialized(false)
        , m_applicationClosed(false)
        , m_shouldClose(false)
    {
    }

    Application::~Application()
    {
        Shutdown();
    }


    void Application::Init(int32 argc, const char* const* argv)
    {
        if (m_applicationInitialized)
        {
            return;
        }

        m_applicationInitialized = true;
        m_applicationClosed = false;

        //- App Init
        Debug::Init();
        COP_LOG_TRACE("Application Init");

        PlatformInit();

        //- Derived App Init
        OnApplication_Init(argc, argv);
    }

    void Application::Shutdown()
    {
        if (m_applicationInitialized == false || m_applicationClosed)
        {
            return;
        }

        m_applicationInitialized = false;
        m_applicationClosed = true;

        //- Derived App Shutdown
        OnApplication_Shutdown();

        COP_LOG_TRACE("Application Shutdown");
    }


    void Application::Run()
    {
        while (m_shouldClose == false)
        {
            PumpMessages();

            OnApplication_Update();
        }
    }

} // namespace Copium
