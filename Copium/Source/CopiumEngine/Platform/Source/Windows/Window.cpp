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

import <utility>;


namespace
{

    using namespace Copium;

    static const wchar_t* k_ClassName = L"CopiumWindowClass";

    static const uint8               k_SwapchainBufferCount = 3;
    static const RenderTextureFormat k_SwapchainFormat      = RenderTextureFormat::BGRA8;

} // namespace


namespace Copium
{

    Window::Window(WindowDesc&& windowDesc)
        : m_desc(std::move(windowDesc))
        , m_isClosing(false)
    {
        HINSTANCE hInstance = GetModuleHandleW(nullptr);

        WNDCLASSEXW wndClassEx = {
            .cbSize        = sizeof(WNDCLASSEXW),
            .style         = CS_DBLCLKS,
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
        ATOM classUniqueId = RegisterClassExW(&wndClassEx);
        COP_ASSERT(classUniqueId != 0);

        // https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
        // https://docs.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
        DWORD dwExStyle = 0;
        DWORD dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW;// | WS_CAPTION | WS_SYSMENU;

        m_hWnd = CreateWindowExW(
            dwExStyle,
            k_ClassName,
            windowDesc.Title.c_str(),
            dwStyle,
            CW_USEDEFAULT, CW_USEDEFAULT,
            windowDesc.Width, windowDesc.Height,
            nullptr,
            nullptr,
            hInstance,
            nullptr
        );
        COP_ASSERT(m_hWnd != nullptr);

        SwapchainDesc swapchainDesc = {
            .WindowNativeHandle = reinterpret_cast<uint64>(m_hWnd),
            .Width              = m_desc.Width,
            .Height             = m_desc.Height,
            .BufferCount        = k_SwapchainBufferCount,
            .Format             = k_SwapchainFormat,
        };
        m_swapchainHandle = Graphics::GetGraphicsDevice()->CreateSwapchain(swapchainDesc);

        WindowHandle windowHandle = static_cast<WindowHandle>(reinterpret_cast<uint64>(m_hWnd));
        EventManager::RegisterWindowCloseCallback(windowHandle, [this]() { OnWindowClose(); });
        EventManager::RegisterWindowResizeCallback(windowHandle, [this](const WindowResizeEvent& evt) { OnWindowResize(evt); });
    }

    Window::~Window()
    {
        Graphics::GetGraphicsDevice()->DestroySwapchain(m_swapchainHandle);
        DestroyWindow(m_hWnd);
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


    int64 __stdcall Window::WndProc(HWND hWnd, uint32 uMsg, uint64 wParam, int64 lParam)
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

} // namespace Copium
