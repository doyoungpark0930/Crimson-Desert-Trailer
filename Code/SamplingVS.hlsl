#include "Common.hlsli"

struct SamplingVertexShaderInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

SamplingPixelShaderInput main(SamplingVertexShaderInput input)
{
    SamplingPixelShaderInput output;
    
    output.position = float4(input.position, 1.0);
    output.texcoord = input.texcoord;

    return output;
}
