#ifndef _HESTERNAL_SIMPLE_LIT_INPUT_INCLUDED
#define _HESTERNAL_SIMPLE_LIT_INPUT_INCLUDED


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


struct InputData
{
    float3 PositionWS;
    float3 NormalWS;
    float2 UV0;
};


InputData InitInputData(Varyings varyings)
{
    InputData inputData;

    inputData.PositionWS = varyings.positionWS;
    inputData.NormalWS   = normalize(varyings.normalWS);
    inputData.UV0        = varyings.uv0;

    return inputData;
}


#endif // _HESTERNAL_SIMPLE_LIT_INPUT_INCLUDED
