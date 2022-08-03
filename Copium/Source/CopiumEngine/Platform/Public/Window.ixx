export module CopiumEngine.Platform.Window;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Event.EventTypes;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Platform.PlatformTypes;

import <memory>;


export namespace Copium
{

    class Window final
    {
        struct PlatformData;

    public:
        Window(WindowDesc&& windowDesc);
        ~Window();

        // [[nodiscard]] uint64 GetNativeHandle() { return reinterpret_cast<uint64>(m_hWnd); }
        [[nodiscard]] bool   IsClosing() const { return m_isClosing; }

        // [[nodiscard]] uint16 GetWidth()  const { return m_desc.Width; }
        // [[nodiscard]] uint16 GetHeight() const { return m_desc.Height; }

        void ProcessEvents();

    private:
        void OnWindowClose();
        void OnWindowResize(const WindowResizeEvent& evt);

    private:
        WindowDesc                    m_desc;
        std::unique_ptr<PlatformData> m_platformData;
        SwapchainHandle               m_swapchainHandle;
        bool                          m_isClosing;
    };

} // export namespace Copium
