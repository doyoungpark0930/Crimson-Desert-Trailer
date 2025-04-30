#ifndef __TILEABLE_NOISE_HLSLI__
#define __TILEABLE_NOISE_HLSLI__

// https://www.shadertoy.com/view/3dVXDc

// Hash by David_Hoskins
#define UI0 1597334673U
#define UI1 3812015801U
#define UI2 uint2(UI0, UI1)
#define UI3 uint3(UI0, UI1, 2798796415U)
#define UIF (1.0 / float(0xffffffffU))

float3 hash33(float3 p)
{
    uint3 q = uint3(int3(p)) * UI3;
    q = (q.x ^ q.y ^ q.z) * UI3;
    return -1. + 2. * float3(q) * UIF;
}

float remap(float x, float a, float b, float c, float d)
{
    return (((x - a) / (b - a)) * (d - c)) + c;
}

// Gradient noise by iq (modified to be tileable)
float gradientNoise3D(float3 x, float freq)
{
    // grid
    float3 p = floor(x);
    float3 w = frac(x);
    
    // quintic interpolant
    float3 u = w * w * w * (w * (w * 6. - 15.) + 10.);
    
    // gradients
    float3 ga = hash33(fmod(p + float3(0., 0., 0.), freq));
    float3 gb = hash33(fmod(p + float3(1., 0., 0.), freq));
    float3 gc = hash33(fmod(p + float3(0., 1., 0.), freq));
    float3 gd = hash33(fmod(p + float3(1., 1., 0.), freq));
    float3 ge = hash33(fmod(p + float3(0., 0., 1.), freq));
    float3 gf = hash33(fmod(p + float3(1., 0., 1.), freq));
    float3 gg = hash33(fmod(p + float3(0., 1., 1.), freq));
    float3 gh = hash33(fmod(p + float3(1., 1., 1.), freq));
    
    // projections
    float va = dot(ga, w - float3(0., 0., 0.));
    float vb = dot(gb, w - float3(1., 0., 0.));
    float vc = dot(gc, w - float3(0., 1., 0.));
    float vd = dot(gd, w - float3(1., 1., 0.));
    float ve = dot(ge, w - float3(0., 0., 1.));
    float vf = dot(gf, w - float3(1., 0., 1.));
    float vg = dot(gg, w - float3(0., 1., 1.));
    float vh = dot(gh, w - float3(1., 1., 1.));
	
    // interpolation
    return va +
           u.x * (vb - va) +
           u.y * (vc - va) +
           u.z * (ve - va) +
           u.x * u.y * (va - vb - vc + vd) +
           u.y * u.z * (va - vc - ve + vg) +
           u.z * u.x * (va - vb - ve + vf) +
           u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);
}

// Fbm(Fractional Brownian motion) for Perlin noise based on iq's blog
float perlinfbm3D(float3 p, float freq, int octaves)
{
    float G = exp2(-.85);
    float amp = 1.;
    float noise = 0.;
    
    [unroll]
    for (int i = 0; i < octaves; ++i)
    {
        noise += amp * gradientNoise3D(p * freq, freq);
        freq *= 2.;
        amp *= G;
    }
    
    return noise;
}

//3D까지는 확실히 맞으나 2D나 1D가 이상하게 나오면 코드 재확인. 왜냐면 챗지피티가 만들어준것이기 때문

float2 hash22(float2 p)
{
    uint2 q = uint2(int2(p)) * UI2;
    q = (q.x ^ q.y) * UI2;
    return -1. + 2. * float2(q) * UIF;
}

float gradientNoise2D(float2 x, float freq)
{
    float2 p = floor(x);
    float2 w = frac(x);

    float2 u = w * w * w * (w * (w * 6. - 15.) + 10.);

    float2 ga = hash22(fmod(p + float2(0., 0.), freq));
    float2 gb = hash22(fmod(p + float2(1., 0.), freq));
    float2 gc = hash22(fmod(p + float2(0., 1.), freq));
    float2 gd = hash22(fmod(p + float2(1., 1.), freq));

    float va = dot(ga, w - float2(0., 0.));
    float vb = dot(gb, w - float2(1., 0.));
    float vc = dot(gc, w - float2(0., 1.));
    float vd = dot(gd, w - float2(1., 1.));

    return va +
           u.x * (vb - va) +
           u.y * (vc - va) +
           u.x * u.y * (va - vb - vc + vd);
}

float perlinfbm2D(float2 x, float freq, int octaves)
{
    float G = exp2(-.85);
    float amp = 1.;
    float noise = 0.;

    [unroll]
    for (int i = 0; i < octaves; ++i)
    {
        noise += amp * gradientNoise2D(x * freq, freq);
        freq *= 2.;
        amp *= G;
    }

    return noise;
}

// Tileable 3D worley noise
float worleyNoise(float3 uv, float freq)
{
    float3 id = floor(uv);
    float3 p = frac(uv);
    
    float minDist = 10000.;
    for (float x = -1.; x <= 1.; ++x)
    {
        for (float y = -1.; y <= 1.; ++y)
        {
            for (float z = -1.; z <= 1.; ++z)
            {
                float3 offset = float3(x, y, z);
                float3 h = hash33(fmod(id + offset, float3(freq, freq, freq))) * .5 + .5;
                h += offset;
                float3 d = p - h;
                minDist = min(minDist, dot(d, d));
            }
        }
    }
    
    // inverted worley noise
    return 1. - minDist;
}

float hash11(float x)
{
    uint h = uint(x) * UI0;
    h = (h ^ (h >> 16)) * UI1;
    return -1.0 + 2.0 * float(h) * UIF;
}

float gradientNoise1D(float x, float freq)
{
    float p = floor(x);
    float w = frac(x);

    float u = w * w * w * (w * (w * 6. - 15.) + 10.);

    float ga = hash11(fmod(p + 0., freq));
    float gb = hash11(fmod(p + 1., freq));

    float va = ga * (w - 0.);
    float vb = gb * (w - 1.);

    return lerp(va, vb, u);
}

float perlinfbm1D(float x, float freq, int octaves)
{
    float G = exp2(-.85);
    float amp = 1.;
    float noise = 0.;

    [unroll]
    for (int i = 0; i < octaves; ++i)
    {
        noise += amp * gradientNoise1D(x * freq, freq);
        freq *= 2.;
        amp *= G;
    }

    return noise;
}


// Tileable Worley fbm inspired by Andrew Schneider's Real-Time Volumetric Cloudscapes
// chapter in GPU Pro 7.
float worleyFbm(float3 p, float freq)
{
    return worleyNoise(p * freq, freq) * .625 +
        	 worleyNoise(p * freq * 2., freq * 2.) * .25 +
        	 worleyNoise(p * freq * 4., freq * 4.) * .125;
}

//https://www.shadertoy.com/view/4ttSWf 에 나온 노이즈
// -------------------- Hash Functions --------------------

float hash1(float2 p)
{
    p = 50.0 * frac(p * 0.3183099);
    return frac(p.x * p.y * (p.x + p.y));
}

float hash1(float n)
{
    return frac(n * 17.0 * frac(n * 0.3183099));
}

float2 hash2(float2 p)
{
    const float2 k = float2(0.3183099, 0.3678794);
    float n = 111.0 * p.x + 113.0 * p.y;
    return frac(n * frac(k * n));
}

// -------------------- Noise Function with Derivatives --------------------

float4 noised(float3 x)
{
    float3 p = floor(x);
    float3 w = frac(x);

    float3 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);
    float3 du = 30.0 * w * w * (w * (w - 2.0) + 1.0);

    float n = p.x + 317.0 * p.y + 157.0 * p.z;

    float a = hash1(n + 0.0);
    float b = hash1(n + 1.0);
    float c = hash1(n + 317.0);
    float d = hash1(n + 318.0);
    float e = hash1(n + 157.0);
    float f = hash1(n + 158.0);
    float g = hash1(n + 474.0);
    float h = hash1(n + 475.0);

    float k0 = a;
    float k1 = b - a;
    float k2 = c - a;
    float k3 = e - a;
    float k4 = a - b - c + d;
    float k5 = a - c - e + g;
    float k6 = a - b - e + f;
    float k7 = -a + b + c - d + e - f - g + h;

    float value = -1.0 + 2.0 * (
        k0 +
        k1 * u.x +
        k2 * u.y +
        k3 * u.z +
        k4 * u.x * u.y +
        k5 * u.y * u.z +
        k6 * u.z * u.x +
        k7 * u.x * u.y * u.z
    );

    float3 deriv = 2.0 * du * float3(
        k1 + k4 * u.y + k6 * u.z + k7 * u.y * u.z,
        k2 + k5 * u.z + k4 * u.x + k7 * u.z * u.x,
        k3 + k6 * u.x + k5 * u.y + k7 * u.x * u.y
    );

    return float4(value, deriv);
}

// -------------------- Matrix Constants --------------------

static const float3x3 m3 = float3x3(
    0.00, 0.80, 0.60,
   -0.80, 0.36, -0.48,
   -0.60, -0.48, 0.64
);

static const float3x3 m3i = float3x3(
    0.00, -0.80, -0.60,
    0.80, 0.36, -0.48,
    0.60, -0.48, 0.64
);

// -------------------- FBM with Derivatives --------------------

float4 fbmd_8(float3 x)
{
    float f = 2.0;
    float s = 0.65;
    float a = 0.0;
    float b = 0.5;
    float3 d = float3(0.0, 0.0, 0.0);

    float3x3 m = float3x3(
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0
    );

    for (int i = 0; i < 8; ++i)
    {
        float4 n = noised(x);
        a += b * n.x;
        if (i < 4)
            d += b * mul(m, n.yzw);
        b *= s;
        x = mul(m3, x) * f;
        m = mul(m3i, m) * f;
    }

    return float4(a, d); //value, gradient
}


#endif // __TILEABLE_NOISE_HLSLI__