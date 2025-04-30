#pragma once

#include "GeometryGenerator.h"
#include "Model.h"
#include "PerlinNoise.hpp"
#include <random>

namespace hlab {

using std::make_shared;
using namespace std;

class GrassModel : public Model {

  public:
    GrassModel() {} // Skip initialization
    GrassModel(ComPtr<ID3D11Device> &device,
               ComPtr<ID3D11DeviceContext> &context) {
        Initialize(device, context);
    }

    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context) {

        // 랜덤함수 설정
        std::mt19937 gen(0);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        // perlinNoise랜덤함수 설정 
        const siv::PerlinNoise::seed_type seed = 123456u;

        const siv::PerlinNoise perlin{seed};

        vector<GrassInstance> &grassInstances = m_instancesCpu;

        for (int i = 0; i < xfreq; i++) {
            for (int j = 0; j < yfreq; j++) {
                const float lengthScale = (dist(gen) * 0.3 + 0.2f) * g_scale;
                const float widthScale = (0.2f + dist(gen) * 0.4f) * g_scale;

                float x = i * dx;
                float z = j * dy;

                float noise = perlin.octave2D_01(x, z, 2); // 2D 노이즈 적용
                if (noise < threshold)
                    continue; // 특정 threshold보다 낮은 경우 skip

                const Vector3 pos =
                    Vector3(x - 2.5f, 0.0f, z - 2.5f) *
                    Vector3(square_xLength, 0.0f, square_zLength);

                const float angle = dist(gen) * 3.141592f; // 바라보는 방향
                const float slope =
                    (dist(gen) - 0.5f) * 2.0f * 3.141592f * 0.1f; // 기본 기울기

                GrassInstance gi;
                gi.instanceWorld =
                    Matrix::CreateScale(widthScale, lengthScale, 1.0f) *
                    Matrix::CreateRotationX(slope) *
                    Matrix::CreateRotationY(angle) *
                    Matrix::CreateTranslation(pos);

                grassInstances.push_back(gi);
            }
        }

        // 쉐이더로 보내기 위해 transpose
        for (auto &i : grassInstances) {
            i.instanceWorld = i.instanceWorld.Transpose();
        }

        // 잔디는 그림자 효과 없음
        m_castShadow = false;

        auto meshData = GeometryGenerator::MakeGrass();

        // Grass에서 사용하는 Vertex의 구조가 다른 Vertex와 다릅니다.
        vector<GrassVertex> grassVertices(meshData.vertices.size());
        for (int i = 0; i < grassVertices.size(); i++) {
            grassVertices[i].posModel = meshData.vertices[i].position;
            grassVertices[i].normalModel = meshData.vertices[i].normalModel;
            grassVertices[i].texcoord = meshData.vertices[i].texcoord;
        }

        // 여러가지 버퍼들 만들기
        D3D11Utils::CreateVertexBuffer(device, grassVertices, m_verticesGpu);

        assert(m_instancesCpu.size() > 0);

        m_instanceCount = UINT(m_instancesCpu.size());
        D3D11Utils::CreateInstanceBuffer(device, m_instancesCpu,
                                         m_instancesGpu);

        m_indexCount = UINT(meshData.indices.size());
        m_vertexCount = UINT(grassVertices.size());
        D3D11Utils::CreateIndexBuffer(device, meshData.indices, m_indexBuffer);

        m_meshConsts.GetCpu().world = Matrix();
        m_meshConsts.Initialize(device);
        m_materialConsts.Initialize(device);
    };

    void Render(ComPtr<ID3D11DeviceContext> &context) override {
        if (m_isVisible) {
            context->VSSetConstantBuffers(0, 1,
                                          Model::m_meshConsts.GetAddressOf());
            context->PSSetConstantBuffers(
                0, 1, Model::m_materialConsts.GetAddressOf());

            ID3D11Buffer *const vertexBuffers[2] = {m_verticesGpu.Get(),
                                                    m_instancesGpu.Get()};
            const UINT strides[2] = {sizeof(GrassVertex),
                                     sizeof(GrassInstance)};
            const UINT offsets[2] = {0, 0};
            context->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
            context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT,
                                      0);
            context->DrawIndexedInstanced(m_indexCount, m_instanceCount, 0, 0,
                                          0);
        }
    };

  public:
    vector<GrassInstance> m_instancesCpu;

    ComPtr<ID3D11Buffer> m_verticesGpu;
    ComPtr<ID3D11Buffer> m_instancesGpu;
    ComPtr<ID3D11Buffer> m_indexBuffer;

    UINT m_indexCount = 0;
    UINT m_vertexCount = 0;
    UINT m_offset = 0;
    UINT m_instanceCount = 0;

    float g_scale = 1.0f;
    float square_xLength = 1.0f;
    float square_zLength = 1.0f;
    int xfreq = 40;
    int yfreq = 40;
    float dx = 0.05f;
    float dy = 0.05f;
    float threshold = 0.5f;
};

} // namespace hlab