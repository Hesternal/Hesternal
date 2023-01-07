module;

#include "Hesternal/Core/Debug.hpp"

#include <Windows.h>
#include <windowsx.h>

// NOTE(v.matushkin): <SAL Warnings> May be it will be fixed once we get import std;
COP_WARNING_PUSH
COP_WARNING_DISABLE_MSVC(4005) // warning C4005: macro redefinition
COP_WARNING_DISABLE_MSVC(5106) // warning C5106: macro redefined with different parameter names
module HesternalEngine.Platform.Window;
COP_WARNING_POP

import HesternalEngine.Event.EventManager;
import HesternalEngine.Graphics;

import <utility>;


namespace
{
    using namespace Hesternal;


    // TODO(v.matushkin): Duplicated in WindowsApplication.cpp
    static constexpr const wchar_t* k_WindowClassName = L"HesternalWindowClass";

    static constexpr uint8               k_SwapchainBufferCount = 3;
    static constexpr RenderTextureFormat k_SwapchainFormat      = RenderTextureFormat::BGRA8;

} // namespace


namespace Hesternal
{

    // struct Window::PlatformData
    // {
    //     HWND Window;
    // };


    Window::Window(WindowDesc&& windowDesc)
        : m_desc(std::move(windowDesc))
        , m_isClosing(false)
    {
        // PlatformData platformData;

        const HINSTANCE hInstance = GetModuleHandleW(nullptr);

        //- Create Window
        HWND hwnd;
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

            hwnd = CreateWindowExW(
                dwExStyle,
                k_WindowClassName,
                windowDesc.Title.c_str(),
                dwStyle,
                CW_USEDEFAULT, CW_USEDEFAULT,
                windowWidth, windowHeight,
                nullptr,
                nullptr,
                hInstance,
                nullptr
            );
            COP_ASSERT(hwnd != nullptr);
        }

        SwapchainDesc swapchainDesc = {
            .WindowNativeHandle = reinterpret_cast<uint64>(hwnd),
            .Width              = m_desc.Width,
            .Height             = m_desc.Height,
            .BufferCount        = k_SwapchainBufferCount,
            .Format             = k_SwapchainFormat,
        };
        m_swapchainHandle = Graphics::GetGraphicsDevice()->CreateSwapchain(swapchainDesc);

        // m_platformData = std::make_unique<PlatformData>(platformData);

        m_windowHandle = static_cast<WindowHandle>(reinterpret_cast<uint64>(hwnd));
        EventManager::RegisterWindowCloseCallback(m_windowHandle, [this]() { OnWindowClose(); });
        EventManager::RegisterWindowResizeCallback(m_windowHandle, [this](const WindowResizeEvent& evt) { OnWindowResize(evt); });
    }

    Window::~Window()
    {
        Graphics::GetGraphicsDevice()->DestroySwapchain(m_swapchainHandle);
        DestroyWindow((HWND)m_windowHandle);
        // UnregisterClassW();
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

} // namespace Hesternal
