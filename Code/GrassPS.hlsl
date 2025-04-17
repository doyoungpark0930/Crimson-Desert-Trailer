#include "Common.hlsli" // ���̴������� include ��� ����


cbuffer MaterialConstants : register(b0)
{
    float3 albedoFactor; // baseColor
    float roughnessFactor;
    
    float metallicFactor;
    float aoFactor;
    float2 padding0;
    
    float3 emissionFactor;

    int useAlbedoMap;
    int useNormalMap;
    int useAOMap; // Ambient Occlusion
    int invertNormalMapY;
    int useMetallicMap;
    int useRoughnessMap;
    int useEmissiveMap;
    float padding1;
};

struct GrassPixelInput
{
    float4 posProj : SV_POSITION;
    float3 posWorld : POSITION;
    float3 normalWorld : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 baseColor : COLOR;
};


float4 main(GrassPixelInput input) : SV_TARGET
{
    float3 lightDir = normalize(lights[0].position - input.posWorld);
    
   
    
    // ������ directional light, ����̶� abs ���
    float3 color = input.baseColor * abs(dot(input.normalWorld, lightDir)) * 2;
    return float4(color, 1);
}

/*
float4 main(GrassPixelInput input) : SV_TARGET
{
    float3 pixelToEye = normalize(eyeWorld - input.posWorld);
    
    float3 normalWorld = dot(pixelToEye, input.normalWorld) > 0 ? input.normalWorld : -input.normalWorld; // ���
    
    //float3 lightDir = normalize(float3(0.2, 1.0, 0.0));
    float3 irradiance = irradianceIBLTex.SampleLevel(linearWrapSampler, normalWorld, 0).rgb;
    
    // ������ directional light, ����̶� abs ���
    float3 color = input.baseColor * irradiance * dot(pixelToEye, normalWorld) * strengthIBL;
    return float4(color, 1);
}
*/