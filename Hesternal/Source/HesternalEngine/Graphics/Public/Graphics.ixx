export module HesternalEngine.Graphics;

import HesternalEngine.Assets.Shader;
import HesternalEngine.Assets.Texture;
import HesternalEngine.Graphics.IGraphicsDevice;
import HesternalEngine.Graphics.RenderContext;
import HesternalEngine.Graphics.RenderData;

import <memory>;
import <utility>;
import <vector>;


export namespace Hesternal
{

    class Graphics final
    {
    public:
        static void Init();
        static void Shutdown();

        [[nodiscard]] static IGraphicsDevice* GetGraphicsDevice() noexcept { return m_graphicsDevice.get(); }

        [[nodiscard]] static const std::shared_ptr<Shader>& GetDefaultShader() noexcept { return m_defaultShader; }

        [[nodiscard]] static const std::shared_ptr<Texture>& GetBlackTexture()  noexcept { return m_blackTexture; }
        [[nodiscard]] static const std::shared_ptr<Texture>& GetWhiteTexture()  noexcept { return m_whiteTexture; }
        [[nodiscard]] static const std::shared_ptr<Texture>& GetNormalTexture() noexcept { return m_normalTexture; }

        static void SetDefaultShader(std::shared_ptr<Shader>&& shader) noexcept { m_defaultShader = std::move(shader); }

        static void SetRenderData(RenderData&& renderData) { m_renderContext->SetRenderData(std::move(renderData)); }
        [[nodiscard]] static RenderContext& GetRenderContext() noexcept { return *m_renderContext; }

    private:
        static inline std::unique_ptr<IGraphicsDevice> m_graphicsDevice;

        static inline std::shared_ptr<Shader>          m_defaultShader;

        static inline std::shared_ptr<Texture>         m_blackTexture;
        static inline std::shared_ptr<Texture>         m_whiteTexture;
        static inline std::shared_ptr<Texture>         m_normalTexture;

        static inline std::unique_ptr<RenderContext>   m_renderContext;
    };

} // export namespace Hesternal
