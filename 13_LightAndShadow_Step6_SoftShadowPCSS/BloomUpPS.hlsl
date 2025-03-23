Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

cbuffer SamplingPixelConstantData : register(b0)
{
    float dx;
    float dy;
    float threshold;
    float strength;
    float4 options;
};

struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
    float x = input.texcoord.x;
    float y = input.texcoord.y;
    
    float3 a = g_texture0.Sample(g_sampler, float2(x - dx, y + dy)).rgb;
    float3 b = g_texture0.Sample(g_sampler, float2(x, y + dy)).rgb;
    float3 c = g_texture0.Sample(g_sampler, float2(x + dx, y + dy)).rgb;

    float3 d = g_texture0.Sample(g_sampler, float2(x - dx, y)).rgb;
    float3 e = g_texture0.Sample(g_sampler, float2(x, y)).rgb;
    float3 f = g_texture0.Sample(g_sampler, float2(x + dx, y)).rgb;

    float3 g = g_texture0.Sample(g_sampler, float2(x - dx, y - dy)).rgb;
    float3 h = g_texture0.Sample(g_sampler, float2(x, y - dy)).rgb;
    float3 i = g_texture0.Sample(g_sampler, float2(x + dx, y - dy)).rgb;

    float3 color = e * 4.0;
    color += (b + d + f + h) * 2.0;
    color += (a + c + g + i);
    color *= 1.0 / 16.0;
  
    return float4(color, 1.0);
    //return g_texture0.Sample(g_sampler, input.texcoord);
}
