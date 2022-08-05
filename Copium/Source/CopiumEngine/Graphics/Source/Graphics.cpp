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
                // Mip0
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                // Mip1
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                // Mip2
                0, 0, 0, 0
            };
            TextureDesc textureDesc;
            textureDesc.Name   = "Black";
            textureDesc.Width  = 4;
            textureDesc.Height = 4;
            textureDesc.Format = TextureFormat::RGBA8;
            textureDesc.Data   = std::move(blackTextureData);
            textureDesc.GenerateMipmaps(true);
            textureDesc.SetAnisotropicFilterMode(16);
            textureDesc.SetWrapMode(TextureWrapMode::Repeat);

            m_blackTexture = std::make_shared<Texture>(std::move(textureDesc));
        }

        {
            std::vector<uint8> whiteTextureData = {
                255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                // Mip1
                255, 255, 255, 255, 255, 255, 255, 255,
                255, 255, 255, 255, 255, 255, 255, 255,
                // Mip2
                255, 255, 255, 255, 255, 255, 255, 255
            };
            TextureDesc textureDesc;
            textureDesc.Name   = "White";
            textureDesc.Width  = 4;
            textureDesc.Height = 4;
            textureDesc.Format = TextureFormat::RGBA8;
            textureDesc.Data   = std::move(whiteTextureData);
            textureDesc.GenerateMipmaps(true);
            textureDesc.SetAnisotropicFilterMode(16);
            textureDesc.SetWrapMode(TextureWrapMode::Repeat);

            m_whiteTexture = std::make_shared<Texture>(std::move(textureDesc));
        }

        {
            // NOTE(v.matushkin): Normal with alpha?
            std::vector<uint8> normalTextureData = {
                // Mip0
                127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255,
                127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255,
                127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255,
                127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255, 127, 127, 255, 255,
                // Mip1
                127, 127, 255, 255, 127, 127, 255, 255,
                127, 127, 255, 255, 127, 127, 255, 255,
                // Mip2
                127, 127, 255, 255
            };
            TextureDesc textureDesc;
            textureDesc.Name   = "White";
            textureDesc.Width  = 4;
            textureDesc.Height = 4;
            textureDesc.Format = TextureFormat::RGBA8;
            textureDesc.Data   = std::move(normalTextureData);
            textureDesc.GenerateMipmaps(true);
            textureDesc.SetAnisotropicFilterMode(16);
            textureDesc.SetWrapMode(TextureWrapMode::Repeat);

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
