#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <iostream>
#include <vector>

#include <d3d11.h>
#include <vector>
#include <windows.h>
#include <wrl/client.h>

namespace hlab {

using Microsoft::WRL::ComPtr;

struct Mesh {
    // Mesh Constant
    // uint16_t Material Constant (materialCBV)
    // PSO

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    ComPtr<ID3D11Buffer> vertexConstBuffer;
    ComPtr<ID3D11Buffer> pixelConstBuffer;

    ComPtr<ID3D11Texture2D> albedoTexture;
    ComPtr<ID3D11Texture2D> emissiveTexture; // 빛을 내는 텍스춰 추가
    ComPtr<ID3D11Texture2D> normalTexture;
    ComPtr<ID3D11Texture2D> heightTexture;
    ComPtr<ID3D11Texture2D> aoTexture;
    ComPtr<ID3D11Texture2D> metallicRoughnessTexture;

    ComPtr<ID3D11ShaderResourceView> albedoSRV;
    ComPtr<ID3D11ShaderResourceView> emissiveSRV;
    ComPtr<ID3D11ShaderResourceView> normalSRV;
    ComPtr<ID3D11ShaderResourceView> heightSRV;
    ComPtr<ID3D11ShaderResourceView> aoSRV;
    ComPtr<ID3D11ShaderResourceView> metallicRoughnessSRV;

    UINT indexCount = 0; // Number of indiecs = 3 * number of triangles
    UINT vertexCount = 0;
    UINT stride = 0;
    UINT offset = 0;
};

} // namespace hlab
