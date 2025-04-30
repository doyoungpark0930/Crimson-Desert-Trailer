#pragma once

#include "ConstantBuffers.h"
#include "D3D11Utils.h"
#include "Mesh.h"
#include "MeshData.h"

// 참고: DirectX-Graphics-Sampels
// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Model/Model.h

namespace hlab {

class Model {
  public:
    Model() {}
    Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
          const std::string &basePath, const std::string &filename);
    Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
          const std::vector<MeshData> &meshes);

    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context,
                    const std::string &basePath, const std::string &filename);

    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context,
                    const std::vector<MeshData> &meshes);

    void UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                               ComPtr<ID3D11DeviceContext> &context);

    virtual void Render(ComPtr<ID3D11DeviceContext> &context);

    void TessellatedRender(ComPtr<ID3D11DeviceContext> &context);

    void RenderNormals(ComPtr<ID3D11DeviceContext> &context);

    void UpdateWorldRow(const Matrix &worldRow);

  public:
    Matrix m_worldRow = Matrix();   // Model(Object) To World 행렬
    Matrix m_worldITRow = Matrix(); // InverseTranspose

    ConstantBuffer<MeshConstants> m_meshConsts;
    ConstantBuffer<MaterialConstants> m_materialConsts;

    bool m_drawNormals = false;
    bool m_isVisible = true;
    bool m_castShadow = true;
    bool m_cloud = false;

    float scale = 1.0f;
    Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);


    std::vector<shared_ptr<Mesh>> m_meshes;

};

} // namespace hlab
