export module CopiumEngine.Platform.Window;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Event.EventTypes;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Platform.PlatformTypes;


export namespace Copium
{

    class Window final
    {
        struct PlatformData;

    public:
        Window(WindowDesc&& windowDesc);
        ~Window();

        [[nodiscard]] WindowHandle GetWindowHandle() const { return m_windowHandle; }
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

} // export namespace Copium
