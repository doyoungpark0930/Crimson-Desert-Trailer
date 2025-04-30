#include "Common.hlsli" // 쉐이더에서도 include 사용 가능
#include "TileableNoise.hlsli"

 


struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION; // NDC 변환 후 좌표
    float2 texcoord : TEXCOORD; // 0~1 범위 텍스쳐 좌표
};


cbuffer Consts : register(b3) // b3 주의
{
    float3 lightDir = float3(-0.1246f, 0.2685f, 1.1246f);
    float dummyC;

}


float3 fog(float3 col, float t)
{
    float3 ext = exp2(-t * 0.00025 * float3(1.0, 1.5, 4.0));
    return col * ext + (1.0 - ext) * float3(0.55, 0.55, 0.58);
}

// FBM을 활용해 구름 밀도 필드 생성
float4 cloudsFbm(float3 pos)
{
    
    float3 offset = float3(2.0, 1.1, 1.0) + 0.07 * float3(globalTime, 0.5 * globalTime, -0.15 * globalTime); // 움직임 속도 줄이기
    return fbmd_8(pos * 0.0015 + offset); // 스케일 줄여서 노이즈 더 촘촘하게
}

float4 cloudsMap(float3 pos, out float nnd)
{
    // 중심 고도와 두께 설정
    float centerY = 900.0;
    float thickness = 20.0;

    float d = abs(pos.y - centerY) - thickness;

    float3 gra = float3(0.0, sign(pos.y - centerY), 0.0);

    float4 n = cloudsFbm(pos);

    d += 400.0 * n.x * (0.7 + 0.3 * gra.y);

    if (d > 0.0)
    {
        nnd = 0.0;
        return float4(-d, 0.0, 0.0, 0.0);
    }

    nnd = -d;
    d = min(-d / 100.0, 0.25); // 거리 클램핑도 더 짧게

    return float4(d, gra);
}



float4 renderClouds(float3 ro, float3 rd, float tmin, float tmax, inout float resT)
{
    float4 sum = float4(0.0, 0.0, 0.0, 0.0);

    float tl = (600.0 - ro.y) / rd.y;
    float th = (1200.0 - ro.y) / rd.y;

    if (tl > 0.0)
        tmin = max(tmin, tl);
    else
        return sum;

    if (th > 0.0)
        tmax = min(tmax, th);

    float t = tmin;
    float lastT = -1.0;
    float thickness = 0.0;

    [loop]
    for (int i = 0; i < 100; ++i)
    {
        float3 pos = ro + t * rd;
        float nnd;
        float4 denGra = cloudsMap(pos, nnd);
        float den = denGra.x;

        float dt = max(0.2, 0.011 * t);
        if (den > 0.001)
        {
            float kk;
            cloudsMap(pos + lightDir * 70.0, kk);
            //float kk = 0.0;
            float sha = 1.0 - smoothstep(-200.0, 200.0, kk);
            sha *= 1.5;

            float3 nor = normalize(denGra.yzw);
            float dif = saturate(0.4 + 0.6 * dot(nor, lightDir)) * sha;
            float fre = saturate(1.0 + dot(nor, rd)) * sha;
            float occ = 0.2 + 0.7 * max(1.0 - kk / 200.0, 0.0) + 0.1 * (1.0 - den);

            // lighting
            float3 lin = float3(0.0, 0.0, 0.0);
            lin += float3(0.70, 0.80, 1.00) * (0.5 + 0.5 * nor.y) * occ;
            lin += float3(0.10, 0.40, 0.20) * (0.5 - 0.5 * nor.y) * occ;
            lin += float3(1.00, 0.95, 0.85) * 3.0 * dif * occ + 0.1;

            // color
            float3 col = float3(0.8, 0.8, 0.8) * 0.45;
            col *= lin;

            col = fog(col, t);

            // front to back blending
            float alp = saturate(den * 0.5 * 0.125 * dt);
            col *= alp;
            sum.rgb += col * (1.0 - sum.a);
            sum.a += alp * (1.0 - sum.a);

            thickness += dt * den;

            if (lastT < 0.0)
                lastT = t;
        }
        else
        {
            dt = abs(den) + 0.2;
        }

        t += dt;

        if (sum.a > 0.995 || t > tmax)
            break;
    }

    if (lastT > 0.0)
        resT = min(resT, lastT);

    // 햇빛 강조 추가
    sum.rgb += max(0.0, 1.0 - 0.0125 * thickness) * float3(1.00, 0.60, 0.40) * 0.3
               * pow(saturate(dot(lightDir, rd)), 32.0);

    return saturate(sum);
}
float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
    // 1. 화면 좌표 => NDC (-1~1)로 변환
    float2 p = (input.texcoord) * 2.0 - 1.0;
    p.y *= -1.0f; // DirectX는 Y축 반대
    
    //이걸로 p대체 가능
    //float2 p = (input.position.xy / float2(1280, 720)) * 2.0 - float2(1.0,1.0);
    //p.y *= -1.0;
    

    // 2. Camera 세팅
    float3 ro = float3(0.0f, 401.0, -6.0);
    float3 ta = float3(0.0f, 385.0, 90.0);

    // 3. 카메라 방향 매트릭스 만들기
    float3 forward = normalize(ta - ro);
    float3 right = normalize(cross(float3(0, 1, 0), forward));
    float3 up = cross(forward, right);

    float3x3 camMatrix = float3x3(right, up, forward);

    // 4. 레이 방향 만들기
    float3 rayDir = (mul(normalize(float3(p, 1.5)), camMatrix));

    // 5. raymarch를 통한 구름 렌더링
    float resT = 4000.0f;
    float4 cloudColor = renderClouds(ro, rayDir, 0.0f, resT, resT);
    float3 col = cloudColor.xyz;
    
     // (1) Sun Glare
    float sun = saturate(dot(lightDir, rayDir));
    col += 0.25f * float3(0.8f, 0.4f, 0.2f) * pow(sun, 4.0f);

    // (2) Gamma Correction
    col = pow(clamp(col * 1.1f - 0.02f, 0.0f.xxx, 1.0f.xxx), 0.4545f.xxx);

    // (3) Contrast Enhancement
    col = col * col * (3.0f - 2.0f * col);

    // (4) Color Grading
    col = pow(col, float3(1.0f, 0.92f, 1.0f)); // soft green tone
    col *= float3(1.02f, 0.99f, 0.9f); // slight red tint
    col.z += 0.1f; // bias blue channel
    
    

    // 최종 출력
    return float4(col, cloudColor.w);
}