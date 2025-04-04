#include "Common.hlsli" 

Texture2D g_heightTexture : register(t0);

cbuffer MeshConstants : register(b0)
{
    matrix world; // Model(또는 Object) 좌표계 -> World로 변환
    matrix worldIT; // World의 InverseTranspose
    int useHeightMap;
    float heightScale;
    float2 dummy;
};


struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct HullOut
{
    float3 pos : POSITION;
    float3 normalModel : NORMAL0; // 모델 좌표계의 normal    
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

//vertex position 결정
[domain("quad")]
PixelShaderInput main(PatchConstOutput patchConst,
             float2 uv : SV_DomainLocation,
             const OutputPatch<HullOut, 4> quad)
{
    PixelShaderInput dout;
    
	// Bilinear interpolation.
    float3 v1 = lerp(quad[0].pos, quad[1].pos, uv.x);
    float3 v2 = lerp(quad[2].pos, quad[3].pos, uv.x);
    float3 p = lerp(v1, v2, uv.y);
    
    // 노멀 벡터 보간
    float3 normal1 = lerp(quad[0].normalModel, quad[1].normalModel, uv.x);
    float3 normal2 = lerp(quad[2].normalModel, quad[3].normalModel, uv.x);
    float4 normal = float4(lerp(normal1, normal2, uv.y), 0.0);
    dout.normalWorld = mul(normal, worldIT).xyz;
    dout.normalWorld = normalize(dout.normalWorld); // 정규화

    // 탄젠트 벡터 보간
    float3 tangent1 = lerp(quad[0].tangentModel, quad[1].tangentModel, uv.x);
    float3 tangent2 = lerp(quad[2].tangentModel, quad[3].tangentModel, uv.x);
    float4 tangentWorld = float4(lerp(tangent1, tangent2, uv.y), 0.0f);
    dout.tangentWorld = mul(tangentWorld, world).xyz;
    dout.tangentWorld = normalize(dout.tangentWorld); // 정규화
    
    //texcoord 보간
    float2 texcoord1 = lerp(quad[0].texcoord, quad[1].texcoord, uv.x);
    float2 texcoord2 = lerp(quad[2].texcoord, quad[3].texcoord, uv.x);
    float2 texcoord = lerp(texcoord1, texcoord2, uv.y);
    dout.texcoord = texcoord;
    
    //월드좌표
    float4 posWorld = float4(p, 1.0);
    posWorld = mul(posWorld, world);
    
    if (useHeightMap)
    {
        float3 height = 2 * g_heightTexture.SampleLevel(linearWrapSampler, dout.texcoord, 0.0).r - 1.0;
        // VertexShader에서는 SampleLevel 사용
        posWorld.xyz += height * dout.normalWorld * heightScale;
    }
    
    dout.posWorld = posWorld.xyz;
    
    //SV좌표
    dout.posProj = mul(posWorld, view);
    dout.posProj = mul(dout.posProj, proj);
    
	
    return dout;
}

