module;

#include <memory>
#include <utility>
#include <vector>

module CopiumEngine.Graphics;

import CopiumEngine.Core.CoreTypes;
import CopiumEngine.Graphics.DX11GraphicsDevice;


namespace Copium
{

    void Graphics::Init()
    {
        m_graphicsDevice = std::make_unique<DX11GraphicsDevice>();

        ShaderDesc shaderDesc;
        shaderDesc.RasterizerStateDesc = RasterizerStateDesc::Default();
        shaderDesc.DepthStencilStateDesc = DepthStencilStateDesc::Default();
        shaderDesc.BlendStateDesc = BlendStateDesc::Default();

        shaderDesc.VertexSource = R"(
            cbuffer PerCamera : register(b0)
            {
                float4x4 _CameraView;
                float4x4 _CameraProjection;
            };
            cbuffer PerMesh : register(b1)
            {
                float4x4 _ObjectToWorld;
            };

            struct Attributes
            {
                float3 positionOS : POSITION;
                float3 normalOS   : NORMAL;
                float2 uv0        : TEXCOORD0;
            };
            struct Varyings
            {
                float4 positionCS : SV_POSITION;
                float3 positionWS : VAR_POSITION;
                float3 normalWS   : VAR_NORMAL;
                float2 uv0        : VAR_TEXCOORD0;
            };

            Varyings main(Attributes IN)
            {
                Varyings OUT;

                float4 positionWS = mul(_ObjectToWorld, float4(IN.positionOS, 1.0f));
                OUT.positionCS    = mul(_CameraProjection, mul(_CameraView, positionWS));
                OUT.positionWS    = positionWS.xyz;
                OUT.normalWS      = mul((float3x3) _ObjectToWorld, IN.normalOS);
                OUT.uv0           = IN.uv0.xy;

                return OUT;
            }
        )";
        shaderDesc.FragmentSource = R"(
            Texture2D _BaseColorMap : register(t0);
            sampler   sampler_BaseColorMap : register(s0);

            Texture2D _NormalMap : register(t1);
            sampler   sampler_NormalMap : register(s1);

            struct Varyings
            {
                float4 positionCS : SV_POSITION;
                float3 positionWS : VAR_POSITION;
                float3 normalWS   : VAR_NORMAL;
                float2 uv0        : VAR_TEXCOORD0;
            };

            float4 main(Varyings input) : SV_TARGET
            {
                float4 baseColor = _BaseColorMap.Sample(sampler_BaseColorMap, input.uv0);
                return baseColor;

                // float3 normalMap = _NormalMap.Sample(sampler_NormalMap, input.uv0);
                // return float4(normalMap, 1.0f);

                // float3 normalWS = normalize(input.normalWS);
                // return float4(normalWS, 1);
            }
        )";
        m_defaultShader = std::make_shared<Shader>(std::move(shaderDesc));

        {
            std::vector<uint8> blackTextureData = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            };
            TextureDesc textureDesc = {
                .Name      = "Black",
                .Width     = 4,
                .Height    = 4,
                .Format    = TextureFormat::RGBA8,
                .WrapModeU = TextureWrapMode::Repeat,
                .WrapModeV = TextureWrapMode::Repeat,
                .Data      = std::move(blackTextureData),
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
                .Name      = "White",
                .Width     = 4,
                .Height    = 4,
                .Format    = TextureFormat::RGBA8,
                .WrapModeU = TextureWrapMode::Repeat,
                .WrapModeV = TextureWrapMode::Repeat,
                .Data      = std::move(whiteTextureData),
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
                .Name      = "Normal",
                .Width     = 4,
                .Height    = 4,
                .Format    = TextureFormat::RGBA8,
                .WrapModeU = TextureWrapMode::Repeat,
                .WrapModeV = TextureWrapMode::Repeat,
                .Data      = std::move(normalTextureData),
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
