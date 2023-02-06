#ifndef _HESTERNAL_SIMPLE_LIT_SURFACE_DATA_INCLUDED
#define _HESTERNAL_SIMPLE_LIT_SURFACE_DATA_INCLUDED


struct SurfaceData
{
    float3 Albedo;
    float  Alpha;
    float3 NormalWS;
};


SurfaceData InitSurfaceData(float2 uv0)
{
    SurfaceData surfaceData;

    const float4 albedoAlpha = _BaseColorMap.Sample(sampler_BaseColorMap, uv0);

    surfaceData.Albedo   = albedoAlpha.rgb;
    surfaceData.Alpha    = albedoAlpha.a;
    surfaceData.NormalWS = _NormalMap.Sample(sampler_NormalMap, uv0).xyz;

    return surfaceData;
}


#endif // _HESTERNAL_SIMPLE_LIT_SURFACE_DATA_INCLUDED
