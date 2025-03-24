#pragma once

#include <memory>

#include "Model.h"

// 참고: DirectX-Graphics-Sampels
// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Model/Model.h

namespace hlab {

class ModelInstance {
  public:
  public:
    std::shared_ptr<const Model> m_model;
    // UploadBuffer m_MeshConstantsCPU;
    // ByteAddressBuffer m_MeshConstantsGPU;
    // UniformTransform locator (위치 찾을때 쓰는거, bounding box 등)
};

} // namespace hlab