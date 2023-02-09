#ifndef _HESTERNAL_SIMPLE_LIT_SURFACE_DATA_INCLUDED
#define _HESTERNAL_SIMPLE_LIT_SURFACE_DATA_INCLUDED


struct SurfaceData
{
    float3 Albedo;
    float  Alpha;
    float3 NormalWS;
    float  Metallic;
    float  Roughness;
};


SurfaceData InitSurfaceData(float2 uv0)
{
    SurfaceData surfaceData;

    const float4 albedoAlpha = _BaseColorMap.Sample(sampler_BaseColorMap, uv0);

    surfaceData.Albedo    = albedoAlpha.rgb;
    surfaceData.Alpha     = albedoAlpha.a;
    surfaceData.NormalWS  = _NormalMap.Sample(sampler_NormalMap, uv0).xyz;
    surfaceData.Metallic  = _MetallicMap.Sample(sampler_MetallicMap, uv0).r;
    surfaceData.Roughness = _RoughnessMap.Sample(sampler_RoughnessMap, uv0).r;

    return surfaceData;
}


#endif // _HESTERNAL_SIMPLE_LIT_SURFACE_DATA_INCLUDED
