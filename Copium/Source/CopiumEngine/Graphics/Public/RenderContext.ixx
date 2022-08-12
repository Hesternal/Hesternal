export module CopiumEngine.Graphics.RenderContext;

import CopiumEngine.Graphics.GraphicsTypes;
import CopiumEngine.Graphics.IGraphicsDevice;
import CopiumEngine.Graphics.RenderData;

import <memory>;
import <vector>;


export namespace Copium
{

    class RenderContext final
    {
    public:
        RenderContext(RenderData&& renderData, IGraphicsDevice* graphicsDevice);
        ~RenderContext() = default;

        RenderContext(const RenderContext&) = delete;
        RenderContext& operator=(const RenderContext&) = delete;
        RenderContext(RenderContext&& other) noexcept = default;
        RenderContext& operator=(RenderContext&& other) noexcept = default;

        void BeginRenderPass(RenderPassHandle renderPassHandle) const;
        void DrawEntities() const;

    private:
        RenderData             m_renderData;
        IGraphicsDevice* const m_graphicsDevice;
    };

} // export namespace Copium
