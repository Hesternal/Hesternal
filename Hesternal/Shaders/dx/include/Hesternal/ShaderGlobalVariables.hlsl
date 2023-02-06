#ifndef _HESTERNAL_SHADER_GLOBAL_VARIABLES_INCLUDED
#define _HESTERNAL_SHADER_GLOBAL_VARIABLES_INCLUDED


cbuffer PerCamera : register(b0)
{
    float4x4 _CameraView;
    float4x4 _CameraProjection;
    float4x4 _CameraViewProjection;
};

cbuffer PerDraw : register(b1)
{
    float4x4 _LocalToWorld;
};


#endif // _HESTERNAL_SHADER_GLOBAL_VARIABLES_INCLUDED
