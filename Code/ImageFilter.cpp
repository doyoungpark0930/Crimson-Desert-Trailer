#include "ImageFilter.h"

namespace hlab {

ImageFilter::ImageFilter(ComPtr<ID3D11Device> &device,
                         ComPtr<ID3D11DeviceContext> &context,
                         ComPtr<ID3D11PixelShader> &pixelShader, int width,
                         int height) {
    Initialize(device, context, pixelShader, width, height);
}

void ImageFilter::Initialize(ComPtr<ID3D11Device> &device,
                             ComPtr<ID3D11DeviceContext> &context,
                             ComPtr<ID3D11PixelShader> &pixelShader, int width,
                             int height) {

    ThrowIfFailed(pixelShader.CopyTo(m_pixelShader.GetAddressOf()));

    ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.Width = float(width);
    m_viewport.Height = float(height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    m_constData.dx = 1.0f / width;
    m_constData.dy = 1.0f / height;

    D3D11Utils::CreateConstBuffer(device, m_constData, m_constBuffer);
}

void ImageFilter::UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                                        ComPtr<ID3D11DeviceContext> &context) {

    D3D11Utils::UpdateBuffer(device, context, m_constData, m_constBuffer);
}

void ImageFilter::Render(ComPtr<ID3D11DeviceContext> &context) const {

    assert(m_shaderResources.size() > 0);
    assert(m_renderTargets.size() > 0);

    context->RSSetViewports(1, &m_viewport);
    context->OMSetRenderTargets(UINT(m_renderTargets.size()),
                                m_renderTargets.data(), NULL);
    context->PSSetShader(m_pixelShader.Get(), 0, 0);
    context->PSSetShaderResources(0, UINT(m_shaderResources.size()),
                                  m_shaderResources.data());
    context->PSSetConstantBuffers(0, 1, m_constBuffer.GetAddressOf());
}

void ImageFilter::SetShaderResources(
    const std::vector<ComPtr<ID3D11ShaderResourceView>> &resources) {

    m_shaderResources.clear();
    for (const auto &res : resources) {
        m_shaderResources.push_back(res.Get());
    }
}

void ImageFilter::SetRenderTargets(
    const std::vector<ComPtr<ID3D11RenderTargetView>> &targets) {

    m_renderTargets.clear();
    for (const auto &tar : targets) {
        m_renderTargets.push_back(tar.Get());
    }
}
} // namespace hlab