#ifndef _HESTERNAL_SIMPLE_LIT_LIGHTING_INCLUDED
#define _HESTERNAL_SIMPLE_LIT_LIGHTING_INCLUDED


struct Light
{
    float3 Direction;
    float3 Color;
    float  Attenuation;
};

struct LightingData
{
    float3 Ambient;
    float3 Diffuse;
    float3 Specular;
};


static const float k_AmbientStrength = 0.05f;


Light ToLight(DirectionalLightData directionalLight)
{
    Light light;

    // TODO(v.matushkin): Normalize in c++ code?
    light.Direction   = normalize(-directionalLight.Forward);
    light.Color       = directionalLight.Color;
    light.Attenuation = 1.0f;

    return light;
}

// NOTE(v.matushkin): Why the fuck you can pass only pointLight and positionWS will be initialized to zero?
//   At least may be there is a warning or something, that I do not log?
Light ToLight(PointLightData pointLight, float3 positionWS)
{
    Light light;

    const float3 lightDirection = pointLight.Position - positionWS;

    light.Direction   = normalize(lightDirection);
    light.Color       = pointLight.Color;
    light.Attenuation = SmoothDistanceAttenuation(lightDirection, pointLight.DistanceAttenuationScale);

    return light;
}

Light ToLight(SpotLightData spotLight, float3 positionWS)
{
    Light light;

    light.Direction   = normalize(-spotLight.Forward);
    light.Color       = spotLight.Color;
    light.Attenuation = PunctualLightAttenuation(positionWS, spotLight.Position, light.Direction, spotLight.DistanceAttenuationScale, spotLight.AngleAttenuationScale, spotLight.AngleAttenuationOffset);

    return light;
}


void EvaluateLight(inout LightingData lightingData, InputData inputData, SurfaceData surfaceData, Light light)
{
    const float3 attenuatedLightColor = light.Color * light.Attenuation;

    //- Ambient
    // NOTE(v.matushkin): This can probably explode if there are a lot of lights
    lightingData.Ambient += attenuatedLightColor * k_AmbientStrength;

    //- Diffuse
    const float NdotL = dot(inputData.NormalWS, light.Direction);
    const float diffuseStrength = max(NdotL, 0.0f);
    lightingData.Diffuse += attenuatedLightColor * diffuseStrength;

    //- Specular
    // NOTE(v.matushkin): I have some doubts about this energy conservation thing, but without live shader reload it's hard to play with this
    // - http://disq.us/p/1010r8y
    // - https://www.rorydriscoll.com/2009/01/25/energy-conservation-in-games/
    const float kPi = 3.14159265; // TODO(v.matushkin): Make it a global constant
    const float kShininess = 16.0;

#if 0 // Phong
    const float energyConservation = (2.0f + kShininess) / (2.0f * kPi);
    const float3 reflectDirWS = reflect(-light.Direction, inputData.NormalWS);
    const float specularStrength = energyConservation * pow(max(dot(inputData.ViewDirWS, reflectDirWS), 0.0f), 8);
#else // Blinn-Phong
    const float energyConservation = (8.0f + kShininess) / (8.0f * kPi);
    const float3 halfwayDirWS = normalize(light.Direction + inputData.ViewDirWS);
    const float specularStrength = energyConservation * pow(max(dot(inputData.NormalWS, halfwayDirWS), 0.0f), kShininess);
#endif
    lightingData.Specular += attenuatedLightColor * surfaceData.Metallic * specularStrength;
}


float4 BlinnPhong(InputData inputData, SurfaceData surfaceData)
{
    LightingData lightingData;
    lightingData.Ambient  = 0.0f;
    lightingData.Diffuse  = 0.0f;
    lightingData.Specular = 0.0f;

    for (uint directionalIndex = 0; directionalIndex < _NumDirectionalLights; ++directionalIndex)
    {
        EvaluateLight(lightingData, inputData, surfaceData, ToLight(_DirectionalLights[directionalIndex]));
    }
    for (uint pointIndex = 0; pointIndex < _NumPointLights; ++pointIndex)
    {
        EvaluateLight(lightingData, inputData, surfaceData, ToLight(_PointLights[pointIndex], inputData.PositionWS));
    }
    for (uint spotIndex = 0; spotIndex < _NumSpotLights; ++spotIndex)
    {
        EvaluateLight(lightingData, inputData, surfaceData, ToLight(_SpotLights[spotIndex], inputData.PositionWS));
    }

    const float3 finalLightColor = saturate(lightingData.Ambient + lightingData.Diffuse + lightingData.Specular);

    return float4(saturate(surfaceData.Albedo * finalLightColor), surfaceData.Alpha);
}


#endif // _HESTERNAL_SIMPLE_LIT_LIGHTING_INCLUDED
