module;

#include "Engine/Core/Debug.hpp"
#include "Engine/ImGui/imgui_impl_dx11.h"
#include "Engine/ImGui/imgui_impl_win32.h"

#include <imgui.h>

module CopiumEngine.ImGui.ImGuiContext;

import CopiumEngine.Graphics;
import CopiumEngine.Graphics.DX11GraphicsDevice;


namespace Copium
{

    void ImGuiContext::Init(WindowHandle windowHandle)
    {
        //- Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& imguiIO = ImGui::GetIO();
        imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable
                             | ImGuiConfigFlags_ViewportsEnable;

        //- Setup Dear ImGui style
        ImGui::StyleColorsDark(); // NOTE(v.matushkin): Probably not a good idea to set style here, but rn it doesn't matter

        DX11GraphicsDevice* dx11GraphicsDevice = (DX11GraphicsDevice*)Graphics::GetGraphicsDevice();

        ImGui_ImplWin32_Init((void*)windowHandle);
        ImGui_ImplDX11_Init(dx11GraphicsDevice->GetDevice(), dx11GraphicsDevice->GetDeviceContext());
    }

    void ImGuiContext::Shutdown()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }


    void ImGuiContext::BeginFrame()
    {
        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX11_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiContext::EndFrame()
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

} // namespace Copium
