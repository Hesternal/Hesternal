module;

#include "Engine/Core/Debug.hpp"

#include <Windows.h>

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


    static int64 __stdcall WndProc(HWND hWnd, uint32 uMsg, uint64 wParam, int64 lParam)
    {
        // NOTE(v.matushkin): Is there a way to handle events for no yet created window?
        WindowHandle windowHandle = static_cast<WindowHandle>(reinterpret_cast<uint64>(hWnd));

        switch (uMsg)
        {
            case WM_WINDOWPOSCHANGED:
            {
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
            }
            return 0;

            case WM_CLOSE:
            {
                EventManager::SendWindowCloseEvent(windowHandle);
            }
            return 0;
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
            .style         = CS_DBLCLKS, // Capture double-clicks
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
