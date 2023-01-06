module;

#include "Copium/Core/Defines.hpp"

#include <imgui.h>

module HesternalEditor.Editor.ImGuiRenderPass;

import HesternalEngine.Engine.EngineSettings;
import HesternalEngine.Graphics.RenderPassNames;
import HesternalEngine.ImGui.ImGuiContext;

import HesternalEditor.Editor.EditorSettings;


namespace Hesternal
{

    ImGuiRenderPass::ImGuiRenderPass()
        : IRenderPass("Editor")
        , m_engineColorId(RenderGraphTextureID::Invalid)
        , m_editorColorId(RenderGraphTextureID::Invalid)
        , m_swapchainHandle(SwapchainHandle::Invalid)
        , m_renderPassHandle(RenderPassHandle::Invalid)
        , m_engineColorHandle(RenderTextureHandle::Invalid)
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
        m_engineColorHandle = renderGraph.GetRenderTexture(m_engineColorId);

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

    void ImGuiRenderPass::OnRender(RenderContext& renderContext)
    {
        CommandBuffer& cmd = renderContext.GetCommandBuffer();
        cmd.BeginSample(GetName());

        renderContext.BeginRenderPass(m_renderPassHandle);
        ImGuiContext::Render(cmd);

        cmd.EndSample();
    }

} // namespace Hesternal
