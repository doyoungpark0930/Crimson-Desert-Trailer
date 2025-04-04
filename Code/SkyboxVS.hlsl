#include "Common.hlsli"

struct SkyboxPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};

SkyboxPixelShaderInput main(VertexShaderInput input)
{

    SkyboxPixelShaderInput output;
    
    output.posModel = input.posModel;
    output.posProj = mul(float4(input.posModel, 1.0), view);
    output.posProj = mul(float4(output.posProj.xyz, 1.0), proj);

    return output;
}
