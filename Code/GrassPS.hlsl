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
    float3 lightDir = lights[0].position - input.posWorld;
    float lightDist = length(lightDir);
    lightDir /= lightDist;
    
    float fallOffEnd = 60.0f; //�ܵ�� FallOffEnd ���⼭ ���� ����
    // Distance attenuation
    float att = saturate((fallOffEnd - lightDist)
                         / (fallOffEnd - lights[0].fallOffStart));
    
    //�׸��� ȿ�� ��Ÿ����. �Ʒ��� ��Ӱ� ���� ���, ���� ����� ���� �׸��� ȿ�� ���������� 4.0*(1-att)��
    input.baseColor = float3(139 / 255.0, 69 / 255.0, 19 / 255.0) * pow(saturate(input.texcoord.y), 4.0 * (1-att));
    
    att = pow(att, 3.0);
    
    // ������ directional light, ����̶� abs ���
    float3 color = input.baseColor * abs(dot(input.normalWorld, lightDir)) * att * lights[0].radiance * 8.0;
    return float4(color, 1);
}