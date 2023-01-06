export module HesternalEngine.Graphics.RenderContext;

import HesternalEngine.Graphics.CommandBuffer;
import HesternalEngine.Graphics.GraphicsBuffer;
import HesternalEngine.Graphics.GraphicsTypes;
import HesternalEngine.Graphics.RenderData;

import <memory>;
import <vector>;
import <utility>;


export namespace Hesternal
{

    class RenderContext final
    {
    public:
        RenderContext();
        ~RenderContext() = default;

        RenderContext(const RenderContext& other) = delete;
        RenderContext& operator=(const RenderContext& other) = delete;
        RenderContext(RenderContext&& other) noexcept = default;
        RenderContext& operator=(RenderContext&& other) noexcept = default;

        [[nodiscard]] CommandBuffer& GetCommandBuffer() { return m_commandBuffer; }

        void SetRenderData(RenderData&& renderData) { m_renderData = std::move(renderData); }
        void NewFrame();

        void BeginRenderPass(RenderPassHandle renderPassHandle);
        void DrawEntities();

    private:
        RenderData     m_renderData;
        CommandBuffer  m_commandBuffer;
        GraphicsBuffer m_perCameraBuffer;
        GraphicsBuffer m_perDrawBuffers;
    };

} // export namespace Hesternal
