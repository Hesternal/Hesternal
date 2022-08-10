export module CopiumEngine.Graphics.IRenderPass;

import CopiumEngine.Graphics.IGraphicsDevice;
import CopiumEngine.Graphics.RenderContext;


export namespace Copium
{

    struct IRenderPass
    {
        IRenderPass() = default;
        virtual ~IRenderPass() = default;

        IRenderPass(const IRenderPass&) = delete;
        IRenderPass& operator=(const IRenderPass&) = delete;
        IRenderPass(IRenderPass&& other) noexcept = default;
        IRenderPass& operator=(IRenderPass&& other) noexcept = default;

        virtual void OnCreate(IGraphicsDevice* graphicsDevice) = 0;
        virtual void OnRender(const RenderContext& renderContext) = 0;
    };

} // export namespace Copium
