#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

cbuffer MeshConstants : register(b0)
{
    matrix world; // Model(또는 Object) 좌표계 -> World로 변환
    matrix worldIT; // World의 InverseTranspose
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

float4 main(VertexShaderInput input) : SV_POSITION
{
    float4 pos = mul(float4(input.posModel, 1.0f), world);
    return mul(pos, viewProj);
}
