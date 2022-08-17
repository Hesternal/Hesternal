Shader "Engine/ImGui"
{
    State
    {
        //- RasterizerState
        Polygon Fill
        Cull Off
        //- DepthStencilState
        DepthTest Off
        DepthWrite On
        DepthCompare Always
        //- BlendState
        BlendOp Add, Add
        Blend SrcAlpha OneMinusSrcAlpha, One OneMinusSrcAlpha
    }
    Vertex
    {
        cbuffer PerCamera : register(b0)
        {
            float4x4 _CameraProjection;
        };

        struct Attributes
        {
            float2 position : POSITION;
            float4 color    : COLOR0;
            float2 uv0      : TEXCOORD0;
        };
        struct Varyings
        {
            float4 position : SV_POSITION;
            float4 color    : COLOR0;
            float2 uv0      : TEXCOORD0;
        };

        Varyings main(Attributes IN)
        {
            Varyings OUT;

            OUT.position = mul(_CameraProjection, float4(IN.position.xy, 0.0f, 1.f));
            OUT.color    = IN.color;
            OUT.uv0      = IN.uv0;

            return OUT;
        }
    }
    Fragment
    {
        Texture2D _Texture      : register(t0);
        sampler sampler_Texture : register(s0);


        struct Varyings
        {
            float4 position : SV_POSITION;
            float4 color    : COLOR0;
            float2 uv0      : TEXCOORD0;
        };


        float4 main(Varyings IN) : SV_TARGET
        {
            float4 outColor = IN.color * _Texture.Sample(sampler_Texture, IN.uv0);
            return outColor;
        }
    }
}
