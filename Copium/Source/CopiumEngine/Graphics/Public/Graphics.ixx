export module CopiumEngine.Graphics;

export import CopiumEngine.Graphics.GraphicsTypes;
export import CopiumEngine.Graphics.IGraphicsDevice;

import CopiumEngine.Assets.Shader;
import CopiumEngine.Assets.Texture;

import <memory>;


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

    private:
        static inline std::unique_ptr<IGraphicsDevice> m_graphicsDevice;

        static inline std::shared_ptr<Shader>          m_defaultShader;

        static inline std::shared_ptr<Texture>         m_blackTexture;
        static inline std::shared_ptr<Texture>         m_whiteTexture;
        static inline std::shared_ptr<Texture>         m_normalTexture;
    };

} // export namespace Copium
