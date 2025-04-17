#pragma once

#include <directxtk/SimpleMath.h>
#include <vector>

namespace hlab {

using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;
using DirectX::SimpleMath::Matrix;

struct Vertex {
    Vector3 position;
    Vector3 normalModel;
    Vector2 texcoord;
    Vector3 tangentModel;
    //Vector3 biTangentModel;
    // biTangent는 쉐이더에서 계산
};

struct GrassVertex {
    Vector3 posModel;
    Vector3 normalModel;
    Vector2 texcoord;

    // 주의: Instance World는 별도의 버퍼로 보냄
};

// GrassVS, grassIL과 일관성이 있어야 합니다.
struct GrassInstance {
    Matrix instanceWorld; // <- Instance 단위의 Model to World 변환
    //float windStrength;
};

} // namespace hlab