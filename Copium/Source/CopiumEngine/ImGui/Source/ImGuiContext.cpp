module;

#include "Engine/Core/Debug.hpp"
#include "Engine/ImGui/imgui_impl_win32.h"

#include <imgui.h>

module CopiumEngine.ImGui.ImGuiContext;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Engine.EngineSettings;
import CopiumEngine.Graphics;
import CopiumEngine.Math;

import <cstring>;
import <utility>;


// NOTE(v.matushkin): DX11 backend, ImGui version 1.88 (18800)
// TODO(v.matushkin): Can't handle ImGuiConfigFlags_ViewportsEnable
//  cause it requires rendering from different RenderPasses and right now I render ImGui in one pass.


namespace
{
    using namespace Copium;


    // Values just taken from ImGui DX11 backend
    static constexpr uint32 k_VertexBufferGrow = 5000;
    static constexpr uint32 k_IndexBufferGrow  = 10000;

    static constexpr uint32 k_VertexElementSize = sizeof(ImDrawVert);
    static constexpr uint32 k_IndexElementSize  = sizeof(ImDrawIdx);
    static constexpr uint32 k_CameraElementSize = sizeof(Float4x4);

    static constexpr int32 k_ImGuiConfigFlags = ImGuiConfigFlags_DockingEnable;
                                              // | ImGuiConfigFlags_ViewportsEnable;
    static constexpr int32 k_ImGuiBackendFlags = ImGuiBackendFlags_RendererHasVtxOffset;
                                               // | ImGuiBackendFlags_RendererHasViewports;
    static const char* k_ImGuiBackendRendererName = "CopiumRenderer";


    static_assert(sizeof(ImGuiTexture) == 8);

} // namespace


namespace Copium
{

    void ImGuiContext::Init(WindowHandle windowHandle, std::shared_ptr<Shader>&& imguiShader)
    {
        m_imguiShader = std::move(imguiShader);

        //- Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& imguiIo = ImGui::GetIO();
        imguiIo.ConfigFlags |= k_ImGuiConfigFlags;

        //- Setup Dear ImGui style
        ImGui::StyleColorsDark(); // NOTE(v.matushkin): Probably not a good idea to set style here, but rn it doesn't matter

        ImGui_ImplWin32_Init((void*)windowHandle);

        _Renderer_Init();
    }

    void ImGuiContext::Shutdown()
    {
        _Renderer_Shutdown();

        //ImGui_ImplDX11_Shutdown();

        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }


    void ImGuiContext::BeginFrame()
    {
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiContext::EndFrame(CommandBuffer& commandBuffer)
    {
        ImGui::Render();
        ImGui::EndFrame();
        _Renderer_DrawData(commandBuffer);

        // Update and Render additional Platform Windows
        //ImGui::UpdatePlatformWindows();
        //ImGui::RenderPlatformWindowsDefault();
    }


    void ImGuiContext::_Renderer_Init()
    {
        ImGuiIO& imguiIo = ImGui::GetIO();
        imguiIo.BackendRendererName = k_ImGuiBackendRendererName;
        imguiIo.BackendFlags |= k_ImGuiBackendFlags;
        // imguiIo.BackendRendererUserData = ;

        //- Create ImGui font texture
        {
            uint8* data;
            int32 width, height;
            // NOTE(v.matushkin): Can't use GetTexDataAsAlpha8() ?
            imguiIo.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

            TextureDesc fontTextureDesc;
            fontTextureDesc.Name   = "ImGui Font";
            fontTextureDesc.Width  = width;
            fontTextureDesc.Height = height;
            fontTextureDesc.Format = TextureFormat::RGBA8;
            fontTextureDesc.GenerateMipmaps(false);
            fontTextureDesc.SetFilterMode(TextureFilterMode::Bilinear);
            fontTextureDesc.SetWrapMode(TextureWrapMode::Repeat);

            const uint32 dataSize = width * height * 4;
            fontTextureDesc.Data.resize(dataSize);
            std::memcpy(fontTextureDesc.Data.data(), data, dataSize);

            m_imguiFontTexture = std::make_unique<Texture>(std::move(fontTextureDesc));

            ImGuiTexture imguiTexture;
            imguiTexture.Parts.Handle = static_cast<uint32>(m_imguiFontTexture->GetHandle());
            imguiTexture.Parts.IsRenderTexture = false;

            imguiIo.Fonts->SetTexID(reinterpret_cast<ImTextureID>(imguiTexture.Whole));
        }

        //- Create camera constant buffer
        m_cameraBuffer = std::make_unique<GraphicsBuffer>(GraphicsBufferDesc::Constant(1, k_CameraElementSize));

        if constexpr ((k_ImGuiConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                      && (k_ImGuiBackendFlags & ImGuiBackendFlags_RendererHasViewports))
        {
            _Renderer_InitPlatformInterface();
        }
    }

    void ImGuiContext::_Renderer_InitPlatformInterface()
    {
        ImGuiPlatformIO& imguiPlatformIo = ImGui::GetPlatformIO();

        imguiPlatformIo.Renderer_CreateWindow = [](ImGuiViewport* imguiViewport)
        {
            COP_LOG_TRACE("ImGui Renderer_CreateWindow");

            SwapchainDesc swapchainDesc = {
                .WindowNativeHandle = reinterpret_cast<uint64>(imguiViewport->PlatformHandle),
                .Width              = static_cast<uint16>(imguiViewport->Size.x),
                .Height             = static_cast<uint16>(imguiViewport->Size.y),
                .BufferCount        = 1,
                .Format             = RenderTextureFormat::BGRA8,
            };

            SwapchainHandle swapchainHandle = Graphics::GetGraphicsDevice()->CreateSwapchain(swapchainDesc);
            imguiViewport->RendererUserData = reinterpret_cast<void*>(swapchainHandle);
        };

        imguiPlatformIo.Renderer_DestroyWindow = [](ImGuiViewport* imguiViewport)
        {
            COP_LOG_TRACE("ImGui Renderer_DestroyWindow");

            if (imguiViewport->RendererUserData != nullptr)
            {
                SwapchainHandle swapchainHandle = static_cast<SwapchainHandle>(reinterpret_cast<uint64>(imguiViewport->PlatformHandle));
                Graphics::GetGraphicsDevice()->DestroySwapchain(swapchainHandle);
                imguiViewport->PlatformHandle = nullptr;
            }
            else
            {
                COP_LOG_WARN("Renderer_DestroyWindow ImGuiViewport::RendererUserData was null");
            }
        };

        imguiPlatformIo.Renderer_SetWindowSize = [](ImGuiViewport* imguiViewport, ImVec2 size)
        {
            COP_LOG_TRACE("ImGui Renderer_SetWindowSize");

            SwapchainHandle swapchainHandle = static_cast<SwapchainHandle>(reinterpret_cast<uint64>(imguiViewport->PlatformHandle));
            Graphics::GetGraphicsDevice()->ResizeSwapchain(swapchainHandle, static_cast<uint16>(size.x), static_cast<uint16>(size.y));
        };
    }

    void ImGuiContext::_Renderer_Shutdown()
    {
        ImGuiIO& imguiIo = ImGui::GetIO();
        imguiIo.Fonts->SetTexID(nullptr);
        imguiIo.BackendRendererName = nullptr;

        // io.BackendRendererUserData = nullptr;
        // ImGui_ImplDX11_ShutdownPlatformInterface();

        m_imguiShader.reset();
        m_imguiFontTexture.reset();
        m_vertexBuffer.reset();
        m_indexBuffer.reset();
        m_cameraBuffer.reset();
    }

    void ImGuiContext::_Renderer_DrawData(CommandBuffer& commandBuffer)
    {
        ImDrawData* imguiDrawData = ImGui::GetDrawData();
        const ImVec2 displaySize = imguiDrawData->DisplaySize;

        // Avoid rendering when minimized
        if (displaySize.x <= 0.0f || displaySize.y <= 0.0f)
        {
            return;
        }

        //- Create and grow vertex/index buffers if needed
        if (const uint32 totalVertexCount = imguiDrawData->TotalVtxCount; m_vertexBuffer == nullptr || totalVertexCount > m_vertexBuffer->GetElementCount())
        {
            const uint32 newElementCount = totalVertexCount + k_VertexBufferGrow;

            if (m_indexBuffer != nullptr)
            {
                COP_LOG_WARN("Resizing ImGui vertex buffer from {:d} to {:d}", m_vertexBuffer->GetElementCount(), newElementCount);
            }

            m_vertexBuffer = std::make_unique<GraphicsBuffer>(GraphicsBufferDesc::Vertex(newElementCount, k_VertexElementSize));
        }
        if (const uint32 totalIndexCount = imguiDrawData->TotalIdxCount; m_indexBuffer == nullptr || totalIndexCount > m_indexBuffer->GetElementCount())
        {
            const uint32 newElementCount = totalIndexCount + k_IndexBufferGrow;

            if (m_indexBuffer != nullptr)
            {
                COP_LOG_WARN("Resizing ImGui index buffer from {:d} to {:d}", m_indexBuffer->GetElementCount(), newElementCount);
            }

            m_indexBuffer = std::make_unique<GraphicsBuffer>(GraphicsBufferDesc::Index(newElementCount, k_IndexElementSize));
        }

        //- Upload vertex/index data into a single contiguous GPU buffer
        const uint32 cmdListCount = imguiDrawData->CmdListsCount;
        {
            std::span<uint8> vertexBufferData = m_vertexBuffer->Map();
            std::span<uint8> indexBufferData = m_indexBuffer->Map();

            uint8* vertexDataPtr = vertexBufferData.data();
            uint8* indexDataPtr = indexBufferData.data();

            for (uint32 i = 0; i < cmdListCount; i++)
            {
                const ImDrawList* const imguiDrawList = imguiDrawData->CmdLists[i];
                const uint32 vertexBufferSize = imguiDrawList->VtxBuffer.Size * k_VertexElementSize;
                const uint32 indexBufferSize = imguiDrawList->IdxBuffer.Size * k_IndexElementSize;

                std::memcpy(vertexDataPtr, imguiDrawList->VtxBuffer.Data, vertexBufferSize);
                std::memcpy(indexDataPtr, imguiDrawList->IdxBuffer.Data, indexBufferSize);

                vertexDataPtr += vertexBufferSize;
                indexDataPtr += indexBufferSize;
            }

            m_vertexBuffer->Unmap();
            m_indexBuffer->Unmap();
        }

        //- Setup camera orthographic projection matrix
        const ImVec2 displayPos = imguiDrawData->DisplayPos;
        {
            const float32 left = displayPos.x;
            const float32 right = displayPos.x + displaySize.x;
            const float32 top = displayPos.y;
            const float32 bottom = displayPos.y + displaySize.y;

            // NOTE(v.matushkin): Z coordinate differs from ImGui backends, not sure if it matters since ImGui draws without Z-test anyway
            const Float4x4 orthoProjection = Float4x4::OrthoOffCenter(left, right, bottom, top, 0.0f, 1.0f);

            std::span<uint8> cameraData = m_cameraBuffer->Map();
            std::memcpy(cameraData.data(), &orthoProjection, k_CameraElementSize);
            m_cameraBuffer->Unmap();
        }

        //- Setup render state
        commandBuffer.BindShader(m_imguiShader.get());
        commandBuffer.BindVertexBuffer(m_vertexBuffer.get(), k_VertexElementSize, 0);
        commandBuffer.BindIndexBuffer(m_indexBuffer.get(), IndexFormat::UInt16);
        commandBuffer.BindConstantBuffer(m_cameraBuffer.get(), 0);
        commandBuffer.SetViewport(Rect(0.0f, 0.0f, displaySize.x, displaySize.y));

        //- Render command lists
        const ImVec2 clipOffset = displayPos;
        uint32 globalVertexOffset = 0;
        uint32 globalIndexOffset = 0;

        for (uint32 cmdListIndex = 0; cmdListIndex < cmdListCount; cmdListIndex++)
        {
            const ImDrawList* const imguiCmdList = imguiDrawData->CmdLists[cmdListIndex];
            const uint32 cmdListBufferCount = imguiCmdList->CmdBuffer.Size;

            for (uint32 cmdBufferIndex = 0; cmdBufferIndex < cmdListBufferCount; cmdBufferIndex++)
            {
                const ImDrawCmd& imguiDrawCommand = imguiCmdList->CmdBuffer[cmdBufferIndex];

                if (imguiDrawCommand.UserCallback != nullptr)
                {
                    COP_LOG_WARN("Don't care about ImGui user callbacks");
                }
                else
                {
                    //-- Project scissor/clipping rectangles into framebuffer space
                    const float32 clipMinX = imguiDrawCommand.ClipRect.x - clipOffset.x;
                    const float32 clipMinY = imguiDrawCommand.ClipRect.y - clipOffset.y;
                    const float32 clipMaxX = imguiDrawCommand.ClipRect.z - clipOffset.x;
                    const float32 clipMaxY = imguiDrawCommand.ClipRect.w - clipOffset.y;

                    if (clipMaxX <= clipMinX || clipMaxY <= clipMinY)
                    {
                        continue;
                    }

                    commandBuffer.SetScissorRect(RectInt::FromMinMax((int32)clipMinX, (int32)clipMinY, (int32)clipMaxX, (int32)clipMaxY));
                    //-- Bind Texture
                    ImGuiTexture textureHandle;
                    textureHandle.Whole = reinterpret_cast<uint64>(imguiDrawCommand.GetTexID());

                    if (textureHandle.Parts.IsRenderTexture)
                    {
                        commandBuffer.BindTexture(static_cast<RenderTextureHandle>(textureHandle.Parts.Handle), 0);
                    }
                    else
                    {
                        commandBuffer.BindTexture(static_cast<TextureHandle>(textureHandle.Parts.Handle), 0);
                    }

                    //-- Draw
                    const uint32 firstIndex = imguiDrawCommand.IdxOffset + globalIndexOffset;
                    const uint32 vertexOffset = imguiDrawCommand.VtxOffset + globalVertexOffset;
                    commandBuffer.DrawIndexed(imguiDrawCommand.ElemCount, firstIndex, vertexOffset);
                }
            }

            globalVertexOffset += imguiCmdList->VtxBuffer.Size;
            globalIndexOffset += imguiCmdList->IdxBuffer.Size;
        }

        // NOTE(v.matushkin): Restore viewport, since EngineRenderPass doesn't have access to IGraphicsDevice
        EngineSettings& engineSettings = EngineSettings::Get();
        const float32 viewportWidth = static_cast<float32>(engineSettings.RenderWidth);
        const float32 viewportHeight = static_cast<float32>(engineSettings.RenderHeight);
        commandBuffer.SetViewport(Rect(0.0f, 0.0f, viewportWidth, viewportHeight));
    }

} // namespace Copium
