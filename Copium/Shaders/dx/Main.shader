Shader "Engine/Main"
{
    State
    {
        Polygon Fill
        Cull Back
        DepthTest On
        DepthWrite On
    }
    Vertex
    {
        cbuffer PerCamera : register(b0)
        {
            float4x4 _CameraView;
            float4x4 _CameraProjection;
        };
        cbuffer PerDraw : register(b1)
        {
            float4x4 _LocalToWorld;
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

            float4 positionWS = mul(_LocalToWorld, float4(IN.positionOS, 1.0f));
            OUT.positionCS    = mul(_CameraProjection, mul(_CameraView, positionWS));
            OUT.positionWS    = positionWS.xyz;
            OUT.normalWS      = mul((float3x3) _LocalToWorld, IN.normalOS);
            OUT.uv0           = IN.uv0.xy;

            return OUT;
        }
    }
    Fragment
    {
        Texture2D _BaseColorMap        : register(t0);
        sampler   sampler_BaseColorMap : register(s0);

        Texture2D _NormalMap        : register(t1);
        sampler   sampler_NormalMap : register(s1);


        struct Varyings
        {
            float4 positionCS : SV_POSITION;
            float3 positionWS : VAR_POSITION;
            float3 normalWS   : VAR_NORMAL;
            float2 uv0        : VAR_TEXCOORD0;
        };


        float4 main(Varyings IN) : SV_TARGET
        {
            float4 baseColor = _BaseColorMap.Sample(sampler_BaseColorMap, IN.uv0);
            return baseColor;

            // float3 normalMap = _NormalMap.Sample(sampler_NormalMap, IN.uv0);
            // return float4(normalMap, 1.0f);

            // float3 normalWS = normalize(IN.normalWS);
            // return float4(normalWS, 1);
        }
    }
}
