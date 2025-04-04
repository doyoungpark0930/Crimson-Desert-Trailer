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
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};


PatchConstOutput MyPatchConstantFunc(InputPatch<VertexOut, 4> patch,
                                     uint patchID : SV_PrimitiveID)
{
    
    float3 center = (patch[0].pos + patch[1].pos + patch[2].pos + patch[3].pos).xyz * 0.25;
    center = mul(float4(center, 1.0), world).xyz;
    float dist = length(center - eyeWorld);
    float distMin = 0.5;
    float distMax = 30.0;
    float tess = 128.0 * saturate((distMax - dist) / (distMax - distMin)) + 1.0;
    
    PatchConstOutput pt;
    if (dist > 80.0)
    {
        pt.edges[0] = 1.0;
        pt.edges[1] = 1.0;
        pt.edges[2] = 1.0;
        pt.edges[3] = 1.0;
    }
    else if(dist > 50.0)
    {
        pt.edges[0] = 2.0;
        pt.edges[1] = 2.0;
        pt.edges[2] = 2.0;
        pt.edges[3] = 2.0;
    }
    else
    {
        pt.edges[0] = 4.0;
        pt.edges[1] = 4.0;
        pt.edges[2] = 4.0;
        pt.edges[3] = 4.0;
    }

	
    pt.inside[0] = tess;
    pt.inside[1] = tess;


    return pt;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("MyPatchConstantFunc")]
[maxtessfactor(64.0f)]
HullOut main(InputPatch<VertexOut, 4> p,
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


