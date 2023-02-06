Shader "Engine/SimpleLit"
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
        #include "Hesternal/ShaderGlobalVariables.hlsl"
        #include "Hesternal/Transform.hlsl"
        #include "Hesternal/SimpleLit/Input.hlsl"


        Varyings main(Attributes IN)
        {
            Varyings OUT;

            const float3 positionWS = TransformObjectToWorld(IN.positionOS);

            OUT.positionWS = positionWS;
            OUT.positionCS = TransformWorldToClip(positionWS);
            OUT.normalWS   = TransformNormalObjectToWorld(IN.normalOS);
            OUT.uv0        = IN.uv0.xy;

            return OUT;
        }
    }
    Fragment
    {
        Texture2D _BaseColorMap        : register(t0);
        sampler   sampler_BaseColorMap : register(s0);

        Texture2D _NormalMap        : register(t1);
        sampler   sampler_NormalMap : register(s1);


        #include "Hesternal/ShaderGlobalVariables.hlsl"
        #include "Hesternal/SimpleLit/Input.hlsl"
        #include "Hesternal/SimpleLit/SurfaceData.hlsl"


        float4 main(Varyings IN) : SV_TARGET
        {
            const InputData inputData = InitInputData(IN);
            const SurfaceData surfaceData = InitSurfaceData(IN.uv0);

            return float4(surfaceData.Albedo, surfaceData.Alpha);
        }
    }
}
