module CopiumEngine.Graphics.RenderGraph;

import <utility>;


namespace Copium
{

    void RenderGraph::AddRenderPass(std::unique_ptr<IRenderPass>&& renderPass)
    {
        m_renderPasses.push_back(std::move(renderPass));
    }


    void RenderGraph::Build(IGraphicsDevice* graphicsDevice)
    {
        for (const std::unique_ptr<IRenderPass>& renderPass : m_renderPasses)
        {
            renderPass->OnCreate(graphicsDevice);
        }
    }

    void RenderGraph::Execute(const RenderContext& renderContext)
    {
        for (const std::unique_ptr<IRenderPass>& renderPass : m_renderPasses)
        {
            renderPass->OnRender(renderContext);
        }
    }

} // namespace Copium
