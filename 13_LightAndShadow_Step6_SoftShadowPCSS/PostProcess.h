#pragma once

#include "ImageFilter.h"

namespace hlab {
class PostProcess {
  public:
    void
    Initialize(ComPtr<ID3D11Device> &device,
               ComPtr<ID3D11DeviceContext> &context,
               const std::vector<ComPtr<ID3D11ShaderResourceView>> &resources,
               const std::vector<ComPtr<ID3D11RenderTargetView>> &targets,
               const int width, const int height, const int bloomLevels);

    void Render(ComPtr<ID3D11DeviceContext> &context);

    void RenderImageFilter(ComPtr<ID3D11DeviceContext> &context,
                           const ImageFilter &imageFilter);

    void CreateBuffer(ComPtr<ID3D11Device> &device,
                      ComPtr<ID3D11DeviceContext> &context, int width,
                      int height, ComPtr<ID3D11ShaderResourceView> &srv,
                      ComPtr<ID3D11RenderTargetView> &rtv);

  public:
    ImageFilter m_combineFilter;
    vector<ImageFilter> m_bloomDownFilters;
    vector<ImageFilter> m_bloomUpFilters;
    
    shared_ptr<Mesh> m_mesh;

  private:
    vector<ComPtr<ID3D11ShaderResourceView>> m_bloomSRVs;
    vector<ComPtr<ID3D11RenderTargetView>> m_bloomRTVs;
};
} // namespace hlab