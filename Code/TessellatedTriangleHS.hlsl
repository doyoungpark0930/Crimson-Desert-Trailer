#include "Common.hlsli" 

cbuffer MeshConstants : register(b0)
{
    matrix world; // Model(¶Ç´Â Object) ÁÂÇ¥°è -> World·Î º¯È¯
    matrix worldIT; // WorldÀÇ InverseTranspose
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

struct VertexOut
{
    float4 pos : POSITION;
    float3 normalModel : NORMAL0; // ¸ðµ¨ ÁÂÇ¥°èÀÇ normal    
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

struct HullOut
{
    float3 pos : POSITION;
    float3 normalModel : NORMAL0; // ¸ðµ¨ ÁÂÇ¥°èÀÇ normal    
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

struct PatchConstOutput
{
    float edges[3] : SV_TessFactor; // »ï°¢ÇüÀÌ¹Ç·Î 3°³ÀÇ ¿§Áö
    float inside[1] : SV_InsideTessFactor; // ³»ºÎ Tessellation Factor (»ï°¢ÇüÀÌ¹Ç·Î 1°³)
};


PatchConstOutput MyPatchConstantFunc(InputPatch<VertexOut, 3> patch,
                                     uint patchID : SV_PrimitiveID)
{
    
    
    float3 center = (patch[0].pos + patch[1].pos + patch[2].pos) / 3.0;
    center = mul(float4(center, 1.0), world).xyz;
    
    float dist = length(center - eyeWorld);
    float distMin = 0.5;
    float distMax = 10.0;
    float tess = 1.0 * saturate((distMax - dist) / (distMax - distMin)) + 1.0;
    
    PatchConstOutput pt;
    
    if (dist > 10.0)
    {
        pt.edges[0] = 1.0;
        pt.edges[1] = 1.0;
        pt.edges[2] = 1.0;
    }
    else if (dist > 6.0)
    {
        pt.edges[0] = 1.0;
        pt.edges[1] = 1.0;
        pt.edges[2] = 1.0;
    }
    else
    {
        pt.edges[0] = 1.0;
        pt.edges[1] = 1.0;
        pt.edges[2] = 1.0;
    }

	
    pt.inside[0] = tess;


    return pt;
}

// Çæ ½¦ÀÌ´õ
[domain("tri")] // »ï°¢Çü µµ¸ÞÀÎ
[partitioning("integer")] // Integer ºÐÇÒ
[outputtopology("triangle_cw")] // ½Ã°è¹æÇâ »ï°¢Çü
[outputcontrolpoints(3)] // 3°³ÀÇ Á¦¾îÁ¡
[patchconstantfunc("MyPatchConstantFunc")]
[maxtessfactor(64.0f)]
HullOut main(InputPatch<VertexOut, 3> p,
             uint i : SV_OutputControlPointID,
             uint patchId : SV_PrimitiveID)
{
    HullOut hout;
    
    hout.pos = p[i].pos.xyz;
    hout.normalModel = p[i].normalModel;
    hout.texcoord = p[i].texcoord;
    hout.tangentModel = p[i].tangentModel;

    return hout;
}


