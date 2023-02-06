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
    const float3 reflectDirWS = reflect(-light.Direction, inputData.NormalWS);
    lightingData.Specular += attenuatedLightColor * surfaceData.Metallic * pow(max(dot(inputData.ViewDirWS, reflectDirWS), 0.0f), 32);
}


// TODO(v.matushkin): It's not Blinn-Phong yet
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
