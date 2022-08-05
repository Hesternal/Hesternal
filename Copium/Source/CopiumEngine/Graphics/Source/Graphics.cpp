module CopiumEngine.Graphics;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.DX11GraphicsDevice;

import <memory>;
import <utility>;
import <vector>;


namespace Copium
{

    void Graphics::Init()
    {
        m_graphicsDevice = std::make_unique<DX11GraphicsDevice>();

        {
            std::vector<uint8> blackTextureData = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            };
            TextureDesc textureDesc = {
                .Name             = "Black",
                .Width            = 4,
                .Height           = 4,
                .Format           = TextureFormat::RGBA8,
                .FilterMode       = TextureFilterMode::Anisotropic,
                .AnisotropicLevel = 16,
                .WrapModeU        = TextureWrapMode::Repeat,
                .WrapModeV        = TextureWrapMode::Repeat,
                .Data             = std::move(blackTextureData),
            };

            m_blackTexture = std::make_shared<Texture>(std::move(textureDesc));
        }

        {
            std::vector<uint8> whiteTextureData = {
                255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
            };
            TextureDesc textureDesc = {
                .Name             = "White",
                .Width            = 4,
                .Height           = 4,
                .Format           = TextureFormat::RGBA8,
                .FilterMode       = TextureFilterMode::Anisotropic,
                .AnisotropicLevel = 16,
                .WrapModeU        = TextureWrapMode::Repeat,
                .WrapModeV        = TextureWrapMode::Repeat,
                .Data             = std::move(whiteTextureData),
            };

            m_whiteTexture = std::make_shared<Texture>(std::move(textureDesc));
        }

        {
            std::vector<uint8> normalTextureData = {
                127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255,
                127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255,
                127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255,
                127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255,
            };
            TextureDesc textureDesc = {
                .Name             = "Normal",
                .Width            = 4,
                .Height           = 4,
                .Format           = TextureFormat::RGBA8,
                .FilterMode       = TextureFilterMode::Anisotropic,
                .AnisotropicLevel = 16,
                .WrapModeU        = TextureWrapMode::Repeat,
                .WrapModeV        = TextureWrapMode::Repeat,
                .Data             = std::move(normalTextureData),
            };

            m_normalTexture = std::make_shared<Texture>(std::move(textureDesc));
        }
    }

    void Graphics::Shutdown()
    {
        m_defaultShader.reset();

        m_blackTexture.reset();
        m_whiteTexture.reset();
        m_normalTexture.reset();

        m_graphicsDevice.reset();
    }

} // namespace Copium
