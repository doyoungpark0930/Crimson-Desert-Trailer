#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

// Vertex Shader에서도 텍스춰 사용
Texture2D g_heightTexture : register(t0);

cbuffer MeshConstants : register(b0)
{
    matrix world; // Model(또는 Object) 좌표계 -> World로 변환
    matrix worldIT; // World의 InverseTranspose
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

PixelShaderInput main(VertexShaderInput input)
{
    // 뷰 좌표계는 NDC이기 때문에 월드 좌표를 이용해서 조명 계산
    
    PixelShaderInput output;
    
    // Normal 벡터 먼저 변환 (Height Mapping)
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, worldIT).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    // Tangent 벡터는 world로 변환
    float4 tangentWorld = float4(input.tangentModel, 0.0f);
    tangentWorld = mul(tangentWorld, world);

    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, world);
    
    if (useHeightMap)
    {
        // VertexShader에서는 SampleLevel 사용
        // Heightmap은 보통 흑백이라서 마지막에 .r로 float 하나만 사용
        float height = g_heightTexture.SampleLevel(linearClampSampler, input.texcoord, 0).r;
        height = height * 2.0 - 1.0;
        pos += float4(output.normalWorld * height * heightScale, 0.0);
    }

    output.posWorld = pos.xyz; // 월드 위치 따로 저장

    pos = mul(pos, viewProj);

    output.posProj = pos;
    output.texcoord = input.texcoord;
    output.tangentWorld = tangentWorld.xyz;
    
    return output;
}
