export module CopiumEngine.Graphics.RenderContext;

import CopiumEngine.Graphics.CommandBuffer;
import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Graphics.RenderData;

import <memory>;
import <vector>;


export namespace Copium
{

    class RenderContext final
    {
    public:
        RenderContext(RenderData&& renderData, CommandBuffer&& commandBuffer);
        ~RenderContext() = default;

        RenderContext(const RenderContext&) = delete;
        RenderContext& operator=(const RenderContext&) = delete;
        RenderContext(RenderContext&& other) noexcept = default;
        RenderContext& operator=(RenderContext&& other) noexcept = default;

        void BeginRenderPass(RenderPassHandle renderPassHandle);
        void DrawEntities();

    private:
        RenderData    m_renderData;
        CommandBuffer m_commandBuffer;
    };

} // export namespace Copium
