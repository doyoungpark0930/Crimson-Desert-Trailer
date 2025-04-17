#pragma once

#include "GeometryGenerator.h"
#include "Model.h"
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


         // Instances 만들기
        std::mt19937 gen(0);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        vector<GrassInstance> &grassInstances = m_instancesCpu;

        for (int i = 0; i < 5000; i++) {
            const float lengthScale = dist(gen) * 0.1f + 0.15f;
            const float widthScale = 0.05f + dist(gen) * 0.1f;
            const Vector3 pos = Vector3(dist(gen) * 1.0f - 1.0f, 0.0f,
                                        dist(gen) * 1.5f - 1.5f) *
                                5.0f;
            const float angle = dist(gen) * 3.141592f; // 바라보는 방향
            const float slope =
                (dist(gen) - 0.5f) * 2.0f * 3.141592f * 0.1f; // 기본 기울기

            GrassInstance gi;
            gi.instanceWorld =
                Matrix::CreateScale(widthScale, lengthScale, 1.0f)*
                Matrix::CreateRotationX(slope) *
                Matrix::CreateRotationY(angle) *
                Matrix::CreateTranslation(pos);

            grassInstances.push_back(gi);
        }
        // 쉐이더로 보내기 위해 transpose
        for (auto &i : grassInstances) {
            i.instanceWorld = i.instanceWorld.Transpose();
        }


        // 잔디는 그림자맵 만들때 제외 (자체 그림자 효과 구현)
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
            context->VSSetConstantBuffers(0, 1, Model::m_meshConsts.GetAddressOf());
            context->PSSetConstantBuffers(0, 1, Model::m_materialConsts.GetAddressOf());

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
};

} // namespace hlab