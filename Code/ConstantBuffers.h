#pragma once

#include "D3D11Utils.h"
#include <directxtk/SimpleMath.h>

// "Common.hlsli"와 동일해야 함
#define MAX_LIGHTS 1
#define LIGHT_OFF 0x00
#define LIGHT_DIRECTIONAL 0x01
#define LIGHT_POINT 0x02
#define LIGHT_SPOT 0x04
#define LIGHT_SHADOW 0x10

namespace hlab {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;

// DirectX-Graphics-Samples/MiniEngine을 따라서 파일이름 변경
// __declspec(align(256)) : DX12에서는 256 align (예습)

// 주로 Vertex/Geometry 쉐이더에서 사용
__declspec(align(256)) struct MeshConstants {
    Matrix world;
    Matrix worldIT;
    int useHeightMap = 0;
    float heightScale = 0.0f;
    Vector2 dummy;
};

// 주로 Pixel 쉐이더에서 사용
__declspec(align(256)) struct MaterialConstants {

    Vector3 albedoFactor = Vector3(1.0f);
    float roughnessFactor = 1.0f;

    float metallicFactor = 1.0f;
    float aoFactor = 1.0f;
    Vector2 dummy0 = Vector2(0.0f);

    Vector3 emissionFactor = Vector3(0.0f);

    // 여러 옵션들에 uint 플래그 하나만 사용할 수도 있습니다.
    int useAlbedoMap = 0;
    int useNormalMap = 0;
    int useAOMap = 0;
    int invertNormalMapY = 0;
    int useMetallicMap = 0;
    int useRoughnessMap = 0;
    int useEmissiveMap = 0;
    float dummy1 = 0.0f;

    // 참고 flags 구현
    /* union {
        uint32_t flags;
        struct {
            // UV0 or UV1 for each texture
            uint32_t baseColorUV : 1;
            uint32_t metallicRoughnessUV : 1;
            uint32_t occlusionUV : 1;
            uint32_t emissiveUV : 1;
            uint32_t normalUV : 1;

            // Three special modes
            uint32_t twoSided : 1;
            uint32_t alphaTest : 1;
            uint32_t alphaBlend : 1;

            uint32_t _pad : 8;

            uint32_t alphaRef : 16; // half float
        };
    };*/
};

struct Light {
    Vector3 radiance = Vector3(5.0f); // strength
    float fallOffStart = 0.0f;
    Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);
    float fallOffEnd = 400.0f;
    Vector3 position = Vector3(0.0f, 5.0f, -2.0f);
    float spotPower = 6.0f;

    // Light type bitmasking
    // ex) LIGHT_SPOT | LIGHT_SHADOW
    uint32_t type = LIGHT_OFF;
    float radius = 0.0f; // 반지름
    Vector2 dummy;

    Matrix viewProj; // 그림자 렌더링에 필요
    Matrix invProj; // 그림자 렌더링 디버깅용
};

// register(b1) 사용
__declspec(align(256)) struct GlobalConstants {
    Matrix view;
    Matrix proj;
    Matrix invProj; // 역프로젝션행렬
    Matrix viewProj;
    Matrix invViewProj; // Proj -> World

    Vector3 eyeWorld;
    float strengthIBL = 0.138f;

    float windStrength = 1.0f;
    int textureToDraw = 0; // 0: Env, 1: Specular, 2: Irradiance, 그외: 검은색
    float envLodBias = 0.0f; // 환경맵 LodBias
    float lodBias = 2.0f;    // 다른 물체들 LodBias

    float globalTime = 0.0f;
    Vector3 dummy;

    Light lights[MAX_LIGHTS];
};

// register(b3) 사용, PostEffectsPS.hlsl
__declspec(align(256)) struct PostEffectsConstants {
    int mode = 1; // 1: Rendered image, 2: DepthOnly
    float depthScale = 1.0f;
    float fogStrength = 0.0f;
};

__declspec(align(256)) struct VolumeConsts {
    Vector3 lightDir = Vector3(-0.1246f, 0.2685f, 1.1246f);
    float dummyC;
};

template <typename T_CONSTS> class ConstantBuffer {
  public:
    void Initialize(ComPtr<ID3D11Device> &device) {
        D3D11Utils::CreateConstBuffer(device, m_cpu, m_gpu);
    }

    void Upload(ComPtr<ID3D11DeviceContext> &context) {
        D3D11Utils::UpdateBuffer(context, m_cpu, m_gpu);
    }

  public:
    T_CONSTS &GetCpu() { return m_cpu; }
    const auto Get() { return m_gpu.Get(); }
    const auto GetAddressOf() { return m_gpu.GetAddressOf(); }

    T_CONSTS m_cpu;
    ComPtr<ID3D11Buffer> m_gpu;
};

} // namespace hlab
