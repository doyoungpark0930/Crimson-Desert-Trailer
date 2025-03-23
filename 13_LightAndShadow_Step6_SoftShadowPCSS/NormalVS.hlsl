#include "Common.hlsli"

struct NormalGeometryShaderInput
{
    float4 posModel : SV_POSITION;
    float3 normalWorld : NORMAL;
};

NormalGeometryShaderInput main(VertexShaderInput input)
{
    NormalGeometryShaderInput output;

    output.posModel = float4(input.posModel, 1.0);
    output.normalWorld = input.normalModel;

    return output;
}
