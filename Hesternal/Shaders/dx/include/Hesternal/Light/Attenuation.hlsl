#ifndef _HESTERNAL_LIGHT_ATTENUATION_INCLUDED
#define _HESTERNAL_LIGHT_ATTENUATION_INCLUDED


// Refs:
// - Moving Frostbite to PBR
// - Real Shading in Unreal Engine 4, SIGGRAPH 2013


// TODO(v.matushkin): There are probably some redundant distance/direction vectors calculations


float DistanceInverseSquareFalloff(float distanceSquared, float distanceAttenuationScale)
{
    const float factor = distanceSquared * distanceAttenuationScale;
    return saturate(1.0f - factor * factor);
}

float SmoothDistanceInverseSquareFalloff(float distanceSquared, float distanceAttenuationScale)
{
    const float factor = DistanceInverseSquareFalloff(distanceSquared, distanceAttenuationScale);
    return factor * factor;
}

float DistanceAttenuation(float3 unnormalizedLightVector, float distanceAttenuationScale)
{
    const float distanceSquared = length(unnormalizedLightVector);
    float attenuation = 1.0f / max(distanceSquared, 0.01f);
    attenuation *= DistanceInverseSquareFalloff(distanceSquared, distanceAttenuationScale);
    return attenuation;
}

float SmoothDistanceAttenuation(float3 unnormalizedLightVector, float distanceAttenuationScale)
{
    const float distanceSquared = dot(unnormalizedLightVector, unnormalizedLightVector);
    float attenuation = 1.0f / max(distanceSquared, 0.01f * 0.01f);
    attenuation *= SmoothDistanceInverseSquareFalloff(distanceSquared, distanceAttenuationScale);
    return attenuation;
}


float AngleAttenuation(float3 normalizedLightVector, float3 lightDirection, float angleAttenuationScale, float angleAttenuationOffset)
{
    float cd = dot(lightDirection, normalizedLightVector);
    const float attenuation = saturate(cd * angleAttenuationScale + angleAttenuationOffset);
    return attenuation;
}

float SmoothAngleAttenuation(float3 normalizedLightVector, float3 lightDirection, float angleAttenuationScale, float angleAttenuationOffset)
{
    const float attenuation = AngleAttenuation(normalizedLightVector, lightDirection, angleAttenuationScale, angleAttenuationOffset);
    return attenuation * attenuation;
}


float PunctualLightAttenuation(float3 positionWS, float3 lightPositionWS, float3 lightDirection, float distanceAttenuationScale, float angleAttenuationScale, float angleAttenuationOffset)
{
    const float3 unnormalizedLightVector = lightPositionWS - positionWS;
    const float3 L = normalize(unnormalizedLightVector);

    float attenuation = 1;
    attenuation *= DistanceAttenuation(unnormalizedLightVector, distanceAttenuationScale);
    attenuation *= AngleAttenuation(L, lightDirection, angleAttenuationScale, angleAttenuationOffset);

    return attenuation * attenuation;
}


#endif // _HESTERNAL_LIGHT_ATTENUATION_INCLUDED
