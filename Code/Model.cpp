
#include "Model.h"
#include "GeometryGenerator.h"

namespace hlab {

Model::Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
             const std::string &basePath, const std::string &filename) {
    this->Initialize(device, context, basePath, filename);
}

Model::Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
             const std::vector<MeshData> &meshes) {
    this->Initialize(device, context, meshes);
}

void Model::Initialize(ComPtr<ID3D11Device> &device,
                       ComPtr<ID3D11DeviceContext> &context,
                       const std::string &basePath,
                       const std::string &filename) {

    auto meshes = GeometryGenerator::ReadFromFile(basePath, filename);

    Initialize(device, context, meshes);
}

void Model::Initialize(ComPtr<ID3D11Device> &device,
                       ComPtr<ID3D11DeviceContext> &context,
                       const std::vector<MeshData> &meshes) {

    // ConstantBuffer 만들기
    m_meshConstsCPU.world = Matrix();

    D3D11Utils::CreateConstBuffer(device, m_meshConstsCPU, m_meshConstsGPU);
    D3D11Utils::CreateConstBuffer(device, m_materialConstsCPU,
                                  m_materialConstsGPU);

    for (const auto &meshData : meshes) {
        auto newMesh = std::make_shared<Mesh>();
        D3D11Utils::CreateVertexBuffer(device, meshData.vertices,
                                       newMesh->vertexBuffer);
        newMesh->indexCount = UINT(meshData.indices.size());
        newMesh->vertexCount = UINT(meshData.vertices.size());
        newMesh->stride = UINT(sizeof(Vertex));
        D3D11Utils::CreateIndexBuffer(device, meshData.indices,
                                      newMesh->indexBuffer);

        if (!meshData.albedoTextureFilename.empty()) {
            D3D11Utils::CreateTexture(
                device, context, meshData.albedoTextureFilename, true,
                newMesh->albedoTexture, newMesh->albedoSRV);
            m_materialConstsCPU.useAlbedoMap = true;
        }

        if (!meshData.emissiveTextureFilename.empty()) {
            D3D11Utils::CreateTexture(
                device, context, meshData.emissiveTextureFilename, true,
                newMesh->emissiveTexture, newMesh->emissiveSRV);
            m_materialConstsCPU.useEmissiveMap = true;
        }

        if (!meshData.normalTextureFilename.empty()) {
            D3D11Utils::CreateTexture(
                device, context, meshData.normalTextureFilename, false,
                newMesh->normalTexture, newMesh->normalSRV);
            m_materialConstsCPU.useNormalMap = true;
        }

        if (!meshData.heightTextureFilename.empty()) {
            D3D11Utils::CreateTexture(
                device, context, meshData.heightTextureFilename, false,
                newMesh->heightTexture, newMesh->heightSRV);
            m_meshConstsCPU.useHeightMap = true;
        }

        if (!meshData.aoTextureFilename.empty()) {
            D3D11Utils::CreateTexture(device, context,
                                      meshData.aoTextureFilename, false,
                                      newMesh->aoTexture, newMesh->aoSRV);
            m_materialConstsCPU.useAOMap = true;
        }

        // GLTF 방식으로 Metallic과 Roughness를 한 텍스춰에 넣음
        // Green : Roughness, Blue : Metallic(Metalness)
        if (!meshData.metallicTextureFilename.empty() ||
            !meshData.roughnessTextureFilename.empty()) {
            D3D11Utils::CreateMetallicRoughnessTexture(
                device, context, meshData.metallicTextureFilename,
                meshData.roughnessTextureFilename,
                newMesh->metallicRoughnessTexture,
                newMesh->metallicRoughnessSRV);
        }

        if (!meshData.metallicTextureFilename.empty()) {
            m_materialConstsCPU.useMetallicMap = true;
        }

        if (!meshData.roughnessTextureFilename.empty()) {
            m_materialConstsCPU.useRoughnessMap = true;
        }

        newMesh->vertexConstBuffer = m_meshConstsGPU;
        newMesh->pixelConstBuffer = m_materialConstsGPU;

        this->m_meshes.push_back(newMesh);
    }
}

void Model::UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11DeviceContext> &context) {
    if (m_isVisible) {
        D3D11Utils::UpdateBuffer(device, context, m_meshConstsCPU,
                                 m_meshConstsGPU);

        D3D11Utils::UpdateBuffer(device, context, m_materialConstsCPU,
                                 m_materialConstsGPU);
    }
}

void Model::Render(ComPtr<ID3D11DeviceContext> &context) {
    if (m_isVisible) {
        for (const auto &mesh : m_meshes) {
            context->VSSetConstantBuffers(
                0, 1, mesh->vertexConstBuffer.GetAddressOf());
            context->PSSetConstantBuffers(
                0, 1, mesh->pixelConstBuffer.GetAddressOf());

            context->VSSetShaderResources(0, 1, mesh->heightSRV.GetAddressOf());

            // 물체 렌더링할 때 여러가지 텍스춰 사용 (t0 부터시작)
            vector<ID3D11ShaderResourceView *> resViews = {
                mesh->albedoSRV.Get(), mesh->normalSRV.Get(), mesh->aoSRV.Get(),
                mesh->metallicRoughnessSRV.Get(), mesh->emissiveSRV.Get()};
            context->PSSetShaderResources(0, UINT(resViews.size()),
                                          resViews.data());

            context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
                                        &mesh->stride, &mesh->offset);

            context->IASetIndexBuffer(mesh->indexBuffer.Get(),
                                      DXGI_FORMAT_R32_UINT, 0);
            context->DrawIndexed(mesh->indexCount, 0, 0);
        }
    }
}

void Model::RenderNormals(ComPtr<ID3D11DeviceContext> &context) {
    for (const auto &mesh : m_meshes) {
        context->GSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
                                    &mesh->stride, &mesh->offset);
        context->Draw(mesh->vertexCount, 0);
    }
}

void Model::UpdateWorldRow(const Matrix &worldRow) {
    this->m_worldRow = worldRow;
    this->m_worldITRow = worldRow;
    m_worldITRow.Translation(Vector3(0.0f));
    m_worldITRow = m_worldITRow.Invert().Transpose();

    m_meshConstsCPU.world = worldRow.Transpose();
    m_meshConstsCPU.worldIT = m_worldITRow.Transpose();
}

} // namespace hlab