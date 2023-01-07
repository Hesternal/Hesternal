Shader "Engine/Triangle"
{
    Vertex
    {
        struct Attributes
        {
            uint vertexID : SV_VertexID;
        };
        struct Varyings
        {
            float4 positionCS : SV_POSITION;
            float3 color      : TEXCOORD0;
        };


        static const float2 vertices[] = {
             0.0f,  0.5f,
             0.5f, -0.5f,
            -0.5f, -0.5f,
        };
        static const float3 colors[] = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
        };


        Varyings main(Attributes IN)
        {
            Varyings OUT;
            OUT.positionCS = float4(vertices[IN.vertexID], 0, 1);
            OUT.color      = colors[IN.vertexID];
            return OUT;
        }
    }
    Fragment
    {
        struct Varyings
        {
            float4 positionCS : SV_POSITION;
            float3 color      : TEXCOORD0;
        };


        float4 main(Varyings IN) : SV_Target
        {
            return float4(IN.color, 1);
        }
    }
}
