module;

// TODO(v.matushkin): Probably shouldn't leak windows.h like this, although it's modules now, idk how much damage it does
#include <Windows.h>

export module CopiumEngine.Platform.Window;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Event.EventManager;
import CopiumEngine.Graphics;
import CopiumEngine.Platform.PlatformTypes;


export namespace Copium
{

    class Window final
    {
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

        static int64 __stdcall WndProc(HWND hWnd, uint32 uMsg, uint64 wParam, int64 lParam);

    private:
        WindowDesc      m_desc;

        HWND            m_hWnd;
        SwapchainHandle m_swapchainHandle;

        bool            m_isClosing;
    };

} // export namespace Copium
