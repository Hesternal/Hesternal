module CopiumEngine.Graphics.EngineRenderPass;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Engine.EngineSettings;
import CopiumEngine.Graphics.IGraphicsDevice;
import CopiumEngine.Graphics.RenderPassNames;


namespace Copium
{

    EngineRenderPass::EngineRenderPass()
        : IRenderPass("Engine")
        , m_engineColorId(RenderGraphTextureID::Invalid)
        , m_engineDepthId(RenderGraphTextureID::Invalid)
        , m_renderPassHandle(RenderPassHandle::Invalid)
        , m_engineColorHandle(RenderTextureHandle::Invalid)
        , m_engineDepthHandle(RenderTextureHandle::Invalid)
    {
    }


    void EngineRenderPass::OnSchedule(RenderGraph& renderGraph)
    {
        m_engineColorId = renderGraph.ScheduleCreate(RenderPassNames::EngineColor);
        m_engineDepthId = renderGraph.ScheduleCreate(RenderPassNames::EngineDepth);
    }

    void EngineRenderPass::OnCreate(RenderGraph& renderGraph)
    {
        EngineSettings& engineSettings = EngineSettings::Get();
        const uint32 renderWidth = engineSettings.RenderWidth;
        const uint32 renderHeight = engineSettings.RenderHeight;

        m_engineColorHandle = renderGraph.CreateRenderTexture(
            m_engineColorId, renderWidth, renderHeight, RenderTextureFormat::BGRA8, RenderTextureClearValue::DefaultColor()
        );

        m_engineDepthHandle = renderGraph.CreateRenderTexture(
            m_engineDepthId, renderWidth, renderHeight, RenderTextureFormat::Depth32, RenderTextureClearValue::DefaultDepthStencil()
        );

        m_renderPassHandle = renderGraph.CreateRenderPass(RenderPassDesc{
            .ColorAttachments = {
                {
                    .RTHandle   = m_engineColorHandle,
                    .LoadAction = AttachmentLoadAction::Clear,
                },
            },
            .DepthStencilAttachment = AttachmentDesc{
                .RTHandle   = m_engineDepthHandle,
                .LoadAction = AttachmentLoadAction::Clear,
            },
            .Subpass = {
                .ColorAttachmentIndices    = {0},
                .UseDepthStencilAttachment = true,
            },
        });
    }

    void EngineRenderPass::OnRender(const RenderContext& renderContext)
    {
        renderContext.BeginRenderPass(m_renderPassHandle);
        renderContext.DrawEntities();
    }

} // namespace Copium