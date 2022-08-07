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


    // TODO(v.matushkin): Duplicated in WindowsApplication.cpp
    static constexpr const wchar_t* k_WindowClassName = L"CopiumWindowClass";

    static constexpr uint8               k_SwapchainBufferCount = 3;
    static constexpr RenderTextureFormat k_SwapchainFormat      = RenderTextureFormat::BGRA8;

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
