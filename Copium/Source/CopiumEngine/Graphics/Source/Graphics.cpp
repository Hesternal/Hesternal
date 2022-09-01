module CopiumEngine.Graphics;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.DX11GraphicsDevice;
import CopiumEngine.Graphics.RenderContext;

import <memory>;
import <utility>;
import <vector>;


namespace Copium
{

    void Graphics::Init()
    {
        m_graphicsDevice = std::make_unique<DX11GraphicsDevice>();
        m_renderContext = std::make_unique<RenderContext>();

        const SamplerDesc defaultSamplerDesc = SamplerDesc::Default();

        //- Black Texture
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
            textureDesc.Format = TextureFormat::RGBA8_UNorm;
            textureDesc.Data   = std::move(blackTextureData);
            textureDesc.GenerateMipmaps(true);

            m_blackTexture = std::make_shared<Texture>(std::move(textureDesc), defaultSamplerDesc);
        }
        //- White Texture
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
            textureDesc.Format = TextureFormat::RGBA8_UNorm;
            textureDesc.Data   = std::move(whiteTextureData);
            textureDesc.GenerateMipmaps(true);

            m_whiteTexture = std::make_shared<Texture>(std::move(textureDesc), defaultSamplerDesc);
        }
        //- Normal Texture
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
            textureDesc.Name   = "Normal";
            textureDesc.Width  = 4;
            textureDesc.Height = 4;
            textureDesc.Format = TextureFormat::RGBA8_UNorm;
            textureDesc.Data   = std::move(normalTextureData);
            textureDesc.GenerateMipmaps(true);

            m_normalTexture = std::make_shared<Texture>(std::move(textureDesc), defaultSamplerDesc);
        }
    }

    void Graphics::Shutdown()
    {
        m_renderContext.reset();

        m_defaultShader.reset();

        m_blackTexture.reset();
        m_whiteTexture.reset();
        m_normalTexture.reset();

        m_graphicsDevice.reset();
    }

} // namespace Copium
