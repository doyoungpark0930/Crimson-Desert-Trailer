#include "Common.hlsli" 

Texture2D g_heightTexture : register(t0);

cbuffer MeshConstants : register(b0)
{
    matrix world; // Model(�Ǵ� Object) ��ǥ�� -> World�� ��ȯ
    matrix worldIT; // World�� InverseTranspose
    int useHeightMap;
    float heightScale;
    float2 dummy;
};


struct PatchConstOutput
{
    float edges[3] : SV_TessFactor;
    float inside[1] : SV_InsideTessFactor;
};

struct HullOut
{
    float3 pos : POSITION;
    float3 normalModel : NORMAL0; // �� ��ǥ���� normal    
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

//vertex position ����
[domain("tri")]
PixelShaderInput main(PatchConstOutput patchConst,
             float3 uvw : SV_DomainLocation,
             const OutputPatch<HullOut, 3> tri)
{
    PixelShaderInput dout;
    
    // Barycentric ����
    float3 p = uvw.x * tri[0].pos + uvw.y * tri[1].pos + uvw.z * tri[2].pos;
    
     // ��� ���� ����
    float3 normal = uvw.x * tri[0].normalModel + uvw.y * tri[1].normalModel + uvw.z * tri[2].normalModel;
    float4 normalWorld = float4(normal, 0.0);
    dout.normalWorld = normalize(mul(normalWorld, worldIT).xyz);

    
    // ź��Ʈ ���� ����
    float3 tangent = uvw.x * tri[0].tangentModel + uvw.y * tri[1].tangentModel + uvw.z * tri[2].tangentModel;
    float4 tangentWorld = float4(tangent, 0.0f);
    dout.tangentWorld = normalize(mul(tangentWorld, world).xyz);
    
    // �ؽ�ó ��ǥ ����
    float2 texcoord = uvw.x * tri[0].texcoord + uvw.y * tri[1].texcoord + uvw.z * tri[2].texcoord;
    dout.texcoord = texcoord;
    
    //������ǥ
    float4 posWorld = float4(p, 1.0);
    posWorld = mul(posWorld, world);
    
    if (useHeightMap)
    {
        float3 height = 2 * g_heightTexture.SampleLevel(linearWrapSampler, dout.texcoord, 0.0).r - 1.0;
        // VertexShader������ SampleLevel ���
        posWorld.xyz += height * dout.normalWorld * heightScale;
    }
    
    dout.posWorld = posWorld.xyz;
    
    //SV��ǥ
    dout.posProj = mul(posWorld, view);
    dout.posProj = mul(dout.posProj, proj);
    
	
    return dout;
}

