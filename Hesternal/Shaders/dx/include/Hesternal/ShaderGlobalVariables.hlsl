#ifndef _HESTERNAL_SHADER_GLOBAL_VARIABLES_INCLUDED
#define _HESTERNAL_SHADER_GLOBAL_VARIABLES_INCLUDED


cbuffer PerFrame : register(b0)
{
    uint  _NumDirectionalLights;
    uint  _NumPointLights;
    uint  _NumSpotLights;
    float __padding0_PerFrame__;
};

cbuffer PerCamera : register(b1)
{
    float4x4 _CameraView;
    float4x4 _CameraProjection;
    float4x4 _CameraViewProjection;
    float3   _CameraPositionWS;
    float    __padding0_PerCamera__;
};

cbuffer PerDraw : register(b2)
{
    float4x4 _LocalToWorld;
};


struct DirectionalLightData
{
    float3 Forward;
    float  __padding0__;
    float3 Color;
    float  __padding1__;
};

struct PointLightData
{
    float3 Position;
    float  DistanceAttenuationScale;
    float3 Color;
    float  __padding0__;
};

struct SpotLightData
{
    float3 Position;
    float  DistanceAttenuationScale;
    float3 Forward;
    float  AngleAttenuationScale;
    float3 Color;
    float  AngleAttenuationOffset;
};


StructuredBuffer<DirectionalLightData> _DirectionalLights : register(t0);
StructuredBuffer<PointLightData>       _PointLights       : register(t1);
StructuredBuffer<SpotLightData>        _SpotLights        : register(t2);


float3 GetCameraPositionWS()
{
    return _CameraPositionWS;
    //return float3(_CameraView._41, _CameraView._42, _CameraView._43);
}

float3 GetViewDirWS(float3 positionWS)
{
    // NOTE(v.matushkin): Can be computed differently if camera is orthographic.
    // NOTE(v.matushkin): What if result is zero? Does it change anything?
    return normalize(GetCameraPositionWS() - positionWS);
}

#endif // _HESTERNAL_SHADER_GLOBAL_VARIABLES_INCLUDED
