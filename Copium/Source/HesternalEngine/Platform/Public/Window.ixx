export module HesternalEngine.Platform.Window;

import Hesternal.Core;

import HesternalEngine.Event.EventTypes;
import HesternalEngine.Graphics.GraphicsTypes;
import HesternalEngine.Platform.PlatformTypes;


export namespace Hesternal
{

    class Window final
    {
        struct PlatformData;

    public:
        Window(WindowDesc&& windowDesc);
        ~Window();

        [[nodiscard]] WindowHandle GetWindowHandle() const { return m_windowHandle; }
        [[nodiscard]] SwapchainHandle GetSwapchainHandle() const { return m_swapchainHandle; }
        [[nodiscard]] bool IsClosing() const { return m_isClosing; }

        // [[nodiscard]] uint16 GetWidth()  const { return m_desc.Width; }
        // [[nodiscard]] uint16 GetHeight() const { return m_desc.Height; }

    private:
        void OnWindowClose();
        void OnWindowResize(const WindowResizeEvent& evt);

    private:
        WindowDesc                    m_desc;
        // std::unique_ptr<PlatformData> m_platformData;
        WindowHandle                  m_windowHandle;
        SwapchainHandle               m_swapchainHandle;
        bool                          m_isClosing;
    };

} // export namespace Hesternal
