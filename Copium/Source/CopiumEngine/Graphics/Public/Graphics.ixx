export module CopiumEngine.Graphics;

import CopiumEngine.Assets.Shader;
import CopiumEngine.Assets.Texture;
import CopiumEngine.Graphics.IGraphicsDevice;
import CopiumEngine.Graphics.RenderContext;
import CopiumEngine.Graphics.RenderData;
import CopiumEngine.Graphics.RenderGraph;

import <memory>;
import <utility>;
import <vector>;


export namespace Copium
{

    class Graphics final
    {
    public:
        static void Init();
        static void Shutdown();

        [[nodiscard]] static IGraphicsDevice* GetGraphicsDevice() { return m_graphicsDevice.get(); }

        [[nodiscard]] static const std::shared_ptr<Shader>& GetDefaultShader() { return m_defaultShader; }

        [[nodiscard]] static const std::shared_ptr<Texture>& GetBlackTexture()  { return m_blackTexture; }
        [[nodiscard]] static const std::shared_ptr<Texture>& GetWhiteTexture()  { return m_whiteTexture; }
        [[nodiscard]] static const std::shared_ptr<Texture>& GetNormalTexture() { return m_normalTexture; }

        [[nodiscard]] static void SetDefaultShader(std::shared_ptr<Shader>&& shader) { m_defaultShader = std::move(shader); }

        [[nodiscard]] static void SetRenderGraph(std::unique_ptr<RenderGraph>&& renderGraph);
        [[nodiscard]] static void SetRenderData(RenderData&& renderData) { m_renderContext->SetRenderData(std::move(renderData)); }

        static void RenderFrame();

    private:
        static inline std::unique_ptr<IGraphicsDevice> m_graphicsDevice;

        static inline std::shared_ptr<Shader>          m_defaultShader;

        static inline std::shared_ptr<Texture>         m_blackTexture;
        static inline std::shared_ptr<Texture>         m_whiteTexture;
        static inline std::shared_ptr<Texture>         m_normalTexture;

        static inline std::unique_ptr<RenderGraph>     m_renderGraph;
        static inline std::unique_ptr<RenderContext>   m_renderContext;
    };

} // export namespace Copium
