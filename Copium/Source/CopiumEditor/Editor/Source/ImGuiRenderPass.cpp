module;

#include "Engine/Core/Defines.hpp"

#include <imgui.h>

module CopiumEditor.Editor.ImGuiRenderPass;

import CopiumEngine.Engine.EngineSettings;
import CopiumEngine.Graphics.RenderPassNames;
import CopiumEngine.ImGui.ImGuiContext;

import CopiumEditor.Editor.EditorSettings;


namespace Copium
{

    ImGuiRenderPass::ImGuiRenderPass()
        : IRenderPass("Editor")
        , m_engineColorNativeRenderTexture(nullptr)
        , m_engineColorId(RenderGraphTextureID::Invalid)
        , m_editorColorId(RenderGraphTextureID::Invalid)
        , m_swapchainHandle(SwapchainHandle::Invalid)
        , m_renderPassHandle(RenderPassHandle::Invalid)
        , m_editorColorHandle(RenderTextureHandle::Invalid)
    {
    }


    void ImGuiRenderPass::OnSchedule(RenderGraph& renderGraph)
    {
        m_engineColorId = renderGraph.ScheduleRead(RenderPassNames::EngineColor);
        m_editorColorId = renderGraph.ScheduleCreate("EditorColor");
    }

    void ImGuiRenderPass::OnCreate(RenderGraph& renderGraph)
    {
        m_engineColorNativeRenderTexture = renderGraph.GetNativeRenderTexture(m_engineColorId);

        EditorSettings& editorSettings = EditorSettings::Get();
        m_editorColorHandle = renderGraph.CreateRenderTexture(
            m_editorColorId, editorSettings.WindowWidth, editorSettings.WindowHeight, RenderTextureFormat::BGRA8, RenderTextureClearValue::DefaultColor()
        );

        RenderPassDesc renderPassDesc = {
            .ColorAttachments = {
                {
                    .RTHandle   = m_editorColorHandle,
                    .LoadAction = AttachmentLoadAction::Clear,
                }
            },
            .DepthStencilAttachment = std::nullopt,
            .Subpass = {
                .ColorAttachmentIndices    = {0},
                .UseDepthStencilAttachment = false,
            },
        };
        m_renderPassHandle = renderGraph.CreateRenderPass(renderPassDesc);
    }

    void ImGuiRenderPass::OnRender(const RenderContext& renderContext)
    {
        renderContext.BeginRenderPass(m_renderPassHandle);
        ImGuiContext::BeginFrame();

        // ImGui::ShowDemoWindow();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        if (ImGui::Begin("Hierarchy"))
        {
            ImGui::End();
        }

        if (ImGui::Begin("Scene"))
        {
            const auto windowSize = ImGui::GetWindowSize();
            if (ImGui::BeginChild("SceneChild", windowSize))
            {
                EngineSettings& engineSettings = EngineSettings::Get();
                ImVec2 engineOutputDimensions(engineSettings.RenderWidth, engineSettings.RenderHeight);

                ImGui::Image(m_engineColorNativeRenderTexture, engineOutputDimensions);

                ImGui::EndChild();
            }

            ImGui::End();
        }

        if (ImGui::Begin("Render Stats"))
        {
            const auto framerate = ImGui::GetIO().Framerate;
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);

            ImGui::End();
        }

        ImGuiContext::EndFrame();
    }

} // namespace Copium
