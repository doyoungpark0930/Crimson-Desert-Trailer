#include "Common.hlsli" // 쉐이더에서도 include 사용 가능


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
    float3 lightDir = lights[0].position - input.posWorld;
    float lightDist = length(lightDir);
    lightDir /= lightDist;
    
    // Distance attenuation
    float att = saturate((lights[0].fallOffEnd - lightDist)
                         / (lights[0].fallOffEnd - lights[0].fallOffStart));
    
    float spotFator = lights[0].type & LIGHT_SPOT
                     ? pow(max(-dot(lightDir, lights[0].direction), 0.0f), lights[0].spotPower*5.0)
                      : 1.0f;
    
    // 간단한 directional light, 양면이라서 abs 사용
    float3 color = input.baseColor * abs(dot(input.normalWorld, lightDir)) *att * lights[0].radiance*spotFator;
    return float4(color, 1);
}