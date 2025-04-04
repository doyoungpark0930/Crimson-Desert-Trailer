#include "Common.hlsli" 

struct VertexOut
{
    float4 pos : POSITION;
    float3 normalModel : NORMAL0; // ¸ðµ¨ ÁÂÇ¥°èÀÇ normal    
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

VertexOut main(VertexShaderInput vin)
{
    VertexOut vout;
	
    vout.pos = float4(vin.posModel, 1.0);
    vout.normalModel = vin.normalModel;
    vout.texcoord = vin.texcoord;
    vout.tangentModel = vin.tangentModel;

    return vout;
}
/*
struct VertexIn
{
    float4 pos : POSITION;
};

struct VertexOut
{
    float4 pos : POSITION;
};

VertexOut main(VertexShaderInput vin)
{
    VertexOut vout;
	
    vout.pos = float4(vin.posModel, 1.0);

    return vout;
}
*/