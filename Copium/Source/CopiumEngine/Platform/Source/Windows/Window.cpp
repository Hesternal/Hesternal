module;

#include "Engine/Core/Debug.hpp"

#include <Windows.h>
#include <windowsx.h>

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get import std;
COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
COP_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
module CopiumEngine.Platform.Window;
COP_WARNING_POP

import CopiumEngine.Event.EventManager;
import CopiumEngine.Graphics;

import <utility>;


namespace
{

    using namespace Copium;


    static constexpr const wchar_t* k_ClassName = L"CopiumWindowClass";

    static constexpr uint8               k_SwapchainBufferCount = 3;
    static constexpr RenderTextureFormat k_SwapchainFormat      = RenderTextureFormat::BGRA8;


    [[nodiscard]] static KeyboardButton win_KeyboardButton(uint64 wParam)
    {
        return static_cast<KeyboardButton>(static_cast<uint16>(wParam));
    }


    static int64 __stdcall WndProc(HWND hWnd, uint32 uMsg, uint64 wParam, int64 lParam)
    {
        // NOTE(v.matushkin): Is there a way to handle events for no yet created window?
        WindowHandle windowHandle = static_cast<WindowHandle>(reinterpret_cast<uint64>(hWnd));

        switch (uMsg)
        {
            // Keyoboard button
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                const KeyboardEvent event = {
                    .Button  = win_KeyboardButton(wParam),
                    .Pressed = uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN,
                };
                EventManager::SendKeyboardEvent(event);

                // return 0;

                break;
            }

            // Mouse Move
            case WM_MOUSEMOVE:
            {
                // Posted to a window when the cursor moves. If the mouse is not captured, the message is posted to the window that contains the cursor.
                // Otherwise, the message is posted to the window that has captured the mouse.
                const MousePositionEvent event = { .Position = Int2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)) };

                EventManager::SendMousePositionEvent(event);

                // return 0; // If an application processes this message, it should return zero.

                break;
            }

            // Mouse Wheel
            case WM_MOUSEWHEEL:
            {
                // NOTE(v.matushkin): Should I divide wheelDelta by 120 ? Because wheel delta multiplied by WHEEL_DELTA=120 by windows

                // const int8 wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? 1 : -1;
                // EventManager::SendMouseWheelEvent(wheelDelta);

                // return 0; // If an application processes this message, it should return zero.

                break;
            }

            // Mouse Button
            case WM_LBUTTONDOWN: // Down Left
            case WM_MBUTTONDOWN: // Down Middle
            case WM_RBUTTONDOWN: // Down Right
            case WM_LBUTTONUP:   // Up Left
            case WM_MBUTTONUP:   // Up Middle
            case WM_RBUTTONUP:   // Up Right
            {
                const bool pressed = uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN;

                MouseButton mouseButton;
                if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
                {
                    mouseButton = MouseButton::Left;
                }
                else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
                {
                    mouseButton = MouseButton::Middle;
                }
                else
                {
                    mouseButton = MouseButton::Right;
                }

                EventManager::SendMouseButtonEvent(MouseButtonEvent{ .Button = mouseButton, .Pressed = pressed });

                // return 0; // If an application processes this message, it should return zero.

                break;
            }

            // Sent to a window whose size, position, or place in the Z order has changed.
            case WM_WINDOWPOSCHANGED:
            {
                // The WM_SIZE and WM_MOVE messages are not sent if an application handles the WM_WINDOWPOSCHANGED message without calling DefWindowProc.
                // It is more efficient to perform any move or size change processing during the WM_WINDOWPOSCHANGED message without calling DefWindowProc.

                uint16 newWidth;
                uint16 newHeight;
                {
                    RECT rect;
                    GetClientRect(hWnd, &rect);
                    ClientToScreen(hWnd, (POINT*)&rect.left);
                    ClientToScreen(hWnd, (POINT*)&rect.right);
                    newWidth  = static_cast<uint16>(rect.right - rect.left);
                    newHeight = static_cast<uint16>(rect.bottom - rect.top);
                }

                // WINDOWPOS* windowPos = reinterpret_cast<WINDOWPOS*>(lParam);
                // COP_LOG_INFO("WM_WINDOWPOSCHANGED: windowPos({}, {}) | clientRect({}, {})", windowPos->cx, windowPos->cy, newWidth, newHeight);

                // WindowDesc& windowDesc = s_Window->m_desc;
                // // windowDesc.Width  = static_cast<uint16>(windowPos->cx);
                // // windowDesc.Height = static_cast<uint16>(windowPos->cy);
                // windowDesc.Width  = static_cast<uint16>(newWidth);
                // windowDesc.Height = static_cast<uint16>(newHeight);

                EventManager::SendWindowResizeEvent(windowHandle, WindowResizeEvent{ .Width = newWidth, .Height = newHeight, });

                return 0;
            }

            case WM_CLOSE:
            {
                EventManager::SendWindowCloseEvent(windowHandle);

                return 0;
            }

            // Background erase.
            case WM_ERASEBKGND:
            {
                // https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-erasebkgnd
                // Seems like it should prevent flickering on resize if return nonzero?
                return 1; // Return nonzero to intdicate that the App will handle background erase
            }
        };

        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

} // namespace


namespace Copium
{

    struct Window::PlatformData
    {
        HWND Window;
    };


    Window::Window(WindowDesc&& windowDesc)
        : m_desc(std::move(windowDesc))
        , m_isClosing(false)
    {
        PlatformData platformData;

        const HINSTANCE hInstance = GetModuleHandleW(nullptr);

        //- Register Window class
        const WNDCLASSEXW wndClassEx = {
            .cbSize        = sizeof(WNDCLASSEXW),
            .style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS, // Capture double-clicks
            .lpfnWndProc   = WndProc,
            .cbClsExtra    = 0,
            .cbWndExtra    = 0,
            .hInstance     = hInstance,
            .hIcon         = nullptr,
            .hCursor       = LoadCursorW(nullptr, IDC_ARROW),
            .hbrBackground = nullptr, // static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)),
            .lpszMenuName  = nullptr,
            .lpszClassName = k_ClassName,
            .hIconSm       = nullptr,
        };
        const ATOM classUniqueId = RegisterClassExW(&wndClassEx);
        COP_ASSERT(classUniqueId != 0);

        //- Create Window
        {
            // https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
            // https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
            const DWORD dwStyle =
                WS_VISIBLE             // The window is initially visible
                | WS_OVERLAPPEDWINDOW; // WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
            const DWORD dwExStyle = 0;

            //-- Adjust window size
            RECT borderRect{};
            // borderRect will be something like this: left -8 | right 8 | top -31 | bottom 8
            AdjustWindowRectEx(&borderRect, dwStyle, false, dwExStyle);

            const uint32 windowWidth = windowDesc.Width + borderRect.right - borderRect.left;
            const uint32 windowHeight = windowDesc.Height + borderRect.bottom - borderRect.top;

            platformData.Window = CreateWindowExW(
                dwExStyle,
                k_ClassName,
                windowDesc.Title.c_str(),
                dwStyle,
                CW_USEDEFAULT, CW_USEDEFAULT,
                windowWidth, windowHeight,
                nullptr,
                nullptr,
                hInstance,
                nullptr
            );
            COP_ASSERT(platformData.Window != nullptr);
        }

        SwapchainDesc swapchainDesc = {
            .WindowNativeHandle = reinterpret_cast<uint64>(platformData.Window),
            .Width              = m_desc.Width,
            .Height             = m_desc.Height,
            .BufferCount        = k_SwapchainBufferCount,
            .Format             = k_SwapchainFormat,
        };
        m_swapchainHandle = Graphics::GetGraphicsDevice()->CreateSwapchain(swapchainDesc);

        m_platformData = std::make_unique<PlatformData>(platformData);

        WindowHandle windowHandle = static_cast<WindowHandle>(reinterpret_cast<uint64>(platformData.Window));
        EventManager::RegisterWindowCloseCallback(windowHandle, [this]() { OnWindowClose(); });
        EventManager::RegisterWindowResizeCallback(windowHandle, [this](const WindowResizeEvent& evt) { OnWindowResize(evt); });
    }

    Window::~Window()
    {
        Graphics::GetGraphicsDevice()->DestroySwapchain(m_swapchainHandle);
        DestroyWindow(m_platformData->Window);
        // UnregisterClassW();
    }


    void Window::ProcessEvents()
    {
        MSG msg;

        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
        {
            // if (msg.message == WM_QUIT)
            // {
            //     // TODO(v.matushkin): Handle
            // }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }


    void Window::OnWindowClose()
    {
        m_isClosing = true;
    }

    void Window::OnWindowResize(const WindowResizeEvent& evt)
    {
        COP_LOG_INFO("Resizing swapchain, width: {:d} | height: {:d}", evt.Width, evt.Height);
        Graphics::GetGraphicsDevice()->ResizeSwapchain(m_swapchainHandle, evt.Width, evt.Height);
    }

} // namespace Copium
