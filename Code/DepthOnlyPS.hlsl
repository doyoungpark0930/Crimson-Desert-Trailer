#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

struct DepthOnlyPixelShaderInput
{
    float4 posProj : SV_POSITION;
};

float4 main(float4 pos : SV_POSITION) : SV_Target0
{
    return float4(1, 1, 1, 1);
}
