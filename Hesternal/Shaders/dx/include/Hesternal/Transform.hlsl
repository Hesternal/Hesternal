#ifndef _HESTERNAL_TRANSFORM_INCLUDED
#define _HESTERNAL_TRANSFORM_INCLUDED

// NOTE(v.matushkin): Looks like compiler will request this include even if it was included before.
//   May be it's not a bad thing, since I cache includes anyway?
//   Or may be it's at least better then to rely on the fact that this file should be included before by some other code.
// #include "ShaderGlobalVariables.hlsl"


float3 TransformObjectToWorld(float3 positionOS)
{
    return mul(_LocalToWorld, float4(positionOS, 1.0f)).xyz;
}

float4 TransformWorldToClip(float3 positionWS)
{
    return mul(_CameraViewProjection, float4(positionWS, 1.0f));
}

float3 TransformNormalObjectToWorld(float3 normalOS)
{
    // NOTE(v.matushkin): Use something like _NormalToWorld matrix for normal transformation?
    //   Since apparently _LocalToWorld will not gonna work for non uniform scale.
    return mul((float3x3) _LocalToWorld, normalOS);
}


#endif // _HESTERNAL_TRANSFORM_INCLUDED
