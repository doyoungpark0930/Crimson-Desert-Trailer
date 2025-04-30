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
    
    float fallOffEnd = 60.0f; //잔디는 FallOffEnd 여기서 따로 설정
    // Distance attenuation
    float att = saturate((fallOffEnd - lightDist)
                         / (fallOffEnd - lights[0].fallOffStart));
    
    //그림자 효과 나타내기. 아래는 어둡게 위는 밝게, 빛과 가까울 수록 그림자 효과 떨어지도록 4.0*(1-att)함
    input.baseColor = float3(139 / 255.0, 69 / 255.0, 19 / 255.0) * pow(saturate(input.texcoord.y), 4.0 * (1-att));
    
    att = pow(att, 3.0);
    
    // 간단한 directional light, 양면이라서 abs 사용
    float3 color = input.baseColor * abs(dot(input.normalWorld, lightDir)) * att * lights[0].radiance * 8.0;
    return float4(color, 1);
}