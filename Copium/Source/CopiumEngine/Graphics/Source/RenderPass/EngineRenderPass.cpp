module CopiumEngine.Graphics.EngineRenderPass;

import CopiumEngine.Engine.Application;
import CopiumEngine.Graphics.IGraphicsDevice;
import CopiumEngine.Platform.Window;


namespace Copium
{

    EngineRenderPass::EngineRenderPass()
        : m_renderPassHandle(RenderPassHandle::Invalid)
        , m_depthRTHandle(RenderTextureHandle::Invalid)
    {
    }


    void EngineRenderPass::OnCreate(IGraphicsDevice* graphicsDevice)
    {
        Window* window = ((EngineApplication*)Application::Get())->GetMainWindow();

        RenderTextureHandle swapchainRTHandle = graphicsDevice->GetSwapchainRenderTexture(window->GetSwapchainHandle());

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
