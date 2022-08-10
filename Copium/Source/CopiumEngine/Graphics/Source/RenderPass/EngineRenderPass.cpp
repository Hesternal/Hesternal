module CopiumEngine.Graphics.EngineRenderPass;

import CopiumEngine.Graphics.IGraphicsDevice;


namespace Copium
{

    EngineRenderPass::EngineRenderPass(SwapchainHandle swapchainHandle)
        : m_swapchainHandle(swapchainHandle)
        , m_renderPassHandle(RenderPassHandle::Invalid)
        , m_depthRTHandle(RenderTextureHandle::Invalid)
    {
    }


    void EngineRenderPass::OnCreate(IGraphicsDevice* graphicsDevice)
    {
        RenderTextureHandle swapchainRTHandle = graphicsDevice->GetSwapchainRenderTexture(m_swapchainHandle);

        RenderTextureDesc depthRenderTextureDesc = {
            .Name       = "Engine Depth",
            .ClearValue = RenderTextureClearValue::DefaultDepthStencil(),
            .Width      = 1280,
            .Height     = 1000,
            .Format     = RenderTextureFormat::Depth32,
            .Usage      = RenderTextureUsage::Default,
        };
        m_depthRTHandle = graphicsDevice->CreateRenderTexture(depthRenderTextureDesc);

        RenderPassDesc renderPassDesc = {
            .ColorAttachments = {
                {
                    .RTHandle   = swapchainRTHandle,
                    .LoadAction = AttachmentLoadAction::Clear,
                }
            },
            .DepthStencilAttachment = AttachmentDesc{
                .RTHandle   = m_depthRTHandle,
                .LoadAction = AttachmentLoadAction::Clear,
            },
            .Subpass = {
                .ColorAttachmentIndices    = {0},
                .UseDepthStencilAttachment = true,
            },
        };
        m_renderPassHandle = graphicsDevice->CreateRenderPass(renderPassDesc);
    }

    void EngineRenderPass::OnRender(const RenderContext& renderContext)
    {
        renderContext.BeginRenderPass(m_renderPassHandle);
        renderContext.DrawEntities();
    }

} // namespace Copium
