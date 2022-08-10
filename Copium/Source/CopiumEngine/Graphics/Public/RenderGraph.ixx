export module CopiumEngine.Graphics.RenderGraph;

import CopiumEngine.Graphics.IRenderPass;
import CopiumEngine.Graphics.IGraphicsDevice;
import CopiumEngine.Graphics.RenderContext;

import <memory>;
import <vector>;


export namespace Copium
{

    class RenderGraph final
    {
    public:
        RenderGraph() = default;
        ~RenderGraph() = default;

        RenderGraph(const RenderGraph&) = delete;
        RenderGraph& operator=(const RenderGraph&) = delete;
        RenderGraph(RenderGraph&& other) noexcept = default;
        RenderGraph& operator=(RenderGraph&& other) noexcept = default;

        void AddRenderPass(std::unique_ptr<IRenderPass>&& renderPass);

        void Build(IGraphicsDevice* graphicsDevice);
        void Execute(const RenderContext& renderContext);

    private:
        std::vector<std::unique_ptr<IRenderPass>> m_renderPasses;
    };

} // export namespace Copium
