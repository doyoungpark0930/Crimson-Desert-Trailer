#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

struct SkyboxPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION; 
};

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

PixelShaderOutput main(SkyboxPixelShaderInput input)
{
    PixelShaderOutput output;
    
    if (textureToDraw == 0)
        output.pixelColor = envIBLTex.SampleLevel(linearWrapSampler, input.posModel.xyz, envLodBias);
    else if (textureToDraw == 1)
        output.pixelColor = specularIBLTex.SampleLevel(linearWrapSampler, input.posModel.xyz, envLodBias);
    else if (textureToDraw == 2)
        output.pixelColor = irradianceIBLTex.SampleLevel(linearWrapSampler, input.posModel.xyz, envLodBias);
    else
        output.pixelColor = float4(135/255, 206/255, 235/255, 1);

    output.pixelColor *= strengthIBL;
    
    return output;
}
