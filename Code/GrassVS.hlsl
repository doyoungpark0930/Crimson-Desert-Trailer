#include "Common.hlsli"

cbuffer MeshConstants : register(b0)
{
    matrix world; // Model(또는 Object) 좌표계 -> World로 변환
    matrix worldIT; // World의 InverseTranspose
    int useHeightMap;
    float heightScale;
    float2 dummyM;
};

struct GrassVertexInput
{
    float3 posModel : POSITION;
    float3 normalModel : NORMAL;
    float2 texcoord : TEXCOORD;
    matrix insWorld : WORLD; // Instance World
    //float windStrength : COLOR; // Const로 이동, 여기서는 테스트 용도
};

struct GrassPixelInput
{
    float4 posProj : SV_POSITION;
    float3 posWorld : POSITION;
    float3 normalWorld : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 baseColor : COLOR;
};

static float3 debugColors[3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };

// https://thebookofshaders.com/13/
float WaveFunc1(float x, float u_time)
{
    // 여러 가지 경우에 대해 보여주기
    return sin(x + u_time);
    
    float amplitude = 1.;
    float frequency = 0.5;
    
    float y = sin(x * frequency);
    float t = 0.01 * (-u_time * 130.0);
    y += sin(x * frequency * 2.1 + t) * 4.5;
    y += sin(x * frequency * 1.72 + t * 1.121) * 4.0;
    y += sin(x * frequency * 2.221 + t * 0.437) * 5.0;
    y += sin(x * frequency * 3.1122 + t * 4.269) * 2.5;
    y *= amplitude * 0.06;
    
    return y;
}

float WaveFunc2(float x, float u_time)
{
    return 0;
    
    float amplitude = 1.;
    float frequency = 0.1;
    
    float y = sin(x * frequency);
    float t = 0.01 * (-u_time * 130.0);
    y += sin(x * frequency * 2.1 + t) * 4.5;
    y += sin(x * frequency * 1.72 + t * 1.121) * 4.0;
    y += sin(x * frequency * 2.221 + t * 0.437) * 5.0;
    y += sin(x * frequency * 3.1122 + t * 4.269) * 2.5;
    y *= amplitude * 0.06;
    
    return y;
}

// Quaternion structure for HLSL
// https://gist.github.com/mattatz/40a91588d5fb38240403f198a938a593

// A given angle of rotation about a given axis
float4 rotate_angle_axis(float angle, float3 axis)
{
    float sn = sin(angle * 0.5);
    float cs = cos(angle * 0.5);
    return float4(axis * sn, cs);
}

float4x4 quaternion_to_matrix(float4 quat)
{
    float4x4 m = float4x4(float4(0, 0, 0, 0), float4(0, 0, 0, 0), float4(0, 0, 0, 0), float4(0, 0, 0, 0));

    float x = quat.x, y = quat.y, z = quat.z, w = quat.w;
    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;

    m[0][0] = 1.0 - (yy + zz);
    m[0][1] = xy - wz;
    m[0][2] = xz + wy;

    m[1][0] = xy + wz;
    m[1][1] = 1.0 - (xx + zz);
    m[1][2] = yz - wx;

    m[2][0] = xz - wy;
    m[2][1] = yz + wx;
    m[2][2] = 1.0 - (xx + yy);

    m[3][3] = 1.0;

    return m;
}

GrassPixelInput main(uint instanceID : SV_InstanceID, // 참고/디버깅용
                     GrassVertexInput input)
{
    GrassPixelInput output;
   
    // 편의상 worldIT == world 라고 가정 (isotropic scaling)
    
    // 주의: input.insWorld, world 두 번 변환

    output.posWorld = mul(float4(input.posModel, 1.0f), input.insWorld).xyz;
    
    // Deform by wind
    float4x4 mWind = float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

    float3 windWorld = float3(WaveFunc1(output.posWorld.x, globalTime), 0, WaveFunc2(output.posWorld.z, globalTime + 123.0f)) * windStrength;
    
    float2 rotCenter = float2(0.0f, 0.1f);
    float2 temp = (input.posModel.xy - rotCenter);
    float coeff = pow(max(0, temp.y), 2.0);
    float3 axis = cross(coeff * windWorld, float3(0, 1, 0));
    float4 q = rotate_angle_axis(windStrength, axis);
    mWind = quaternion_to_matrix(q);


    /*float2 rotCenter = float2(0.0f, -0.5f);
    float2 temp = (input.posModel.xy - rotCenter);
    float coeff = windTrunk * pow(max(0, temp.y), 2.0) * sin(globalTime);
    float2x2 rot = float2x2(cos(coeff), sin(coeff), -sin(coeff), cos(coeff));
    input.posModel.xy = mul(temp, rot);
    input.posModel.xy += rotCenter;*/
    
    output.normalWorld = mul(float4(input.normalModel, 0.0f), input.insWorld).xyz;
    output.normalWorld = mul(float4(output.normalWorld, 0.0f), mWind).xyz;
    output.normalWorld = mul(float4(output.normalWorld, 0.0f), world).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    float3 translation = input.insWorld._m30_m31_m32;
    output.posWorld -= translation;
    
    output.posWorld = mul(float4(output.posWorld, 1.0f), mWind).xyz;
    
    output.posWorld += translation;
    
    output.posWorld = mul(float4(output.posWorld, 1.0f), world).xyz;
    output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
    output.texcoord = input.texcoord;

    output.baseColor = float3(139/255.0, 69/255.0, 19/255.0) * pow(saturate(input.texcoord.y), 3.0);
    // output.baseColor = debugColors[instanceID % 3] * pow(saturate(input.texcoord.y), 3.0);
    
    return output;
}
