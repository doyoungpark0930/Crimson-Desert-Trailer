﻿#pragma once

#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <iostream>
#include <memory>

#include "AppBase.h"
#include "GeometryGenerator.h"
#include "ImageFilter.h"
#include "Model.h"
#include "GrassModel.h"

namespace hlab {

using DirectX::BoundingSphere;
using DirectX::SimpleMath::Vector3;

class ExampleApp : public AppBase {
  public:
    ExampleApp();

    virtual bool Initialize() override;
    virtual void UpdateGUI() override;
    virtual void Update(float dt) override;
    virtual void Render() override;

    void UpdateLights(float dt);

  protected:
    shared_ptr<Model> m_ground;
    shared_ptr<Model> m_mainObj;
    shared_ptr<Model> m_testObj1;
    shared_ptr<GrassModel> m_grass;
    shared_ptr<GrassModel> m_grass1;
    shared_ptr<GrassModel> m_grass2;
    shared_ptr<GrassModel> m_grass3;
    shared_ptr<GrassModel> m_grass4;
    shared_ptr<GrassModel> m_grass5;
    shared_ptr<GrassModel> m_grass6;
    shared_ptr<Model> m_Cloud;
    shared_ptr<Model> m_lightSphere[MAX_LIGHTS];
    shared_ptr<Model> m_skybox;
    shared_ptr<Model> m_mountain;
    shared_ptr<Model> m_cursorSphere;
    shared_ptr<Model> m_screenSquare;

    BoundingSphere m_mainBoundingSphere;

    bool m_usePerspectiveProjection = true;

    float m_windStrength = 1.0f; //잔디 바람세기

    bool m_grassVisible= true;

    // 거울
    //shared_ptr<Model> m_mirror;
    //DirectX::SimpleMath::Plane m_mirrorPlane;
    //float m_mirrorAlpha = 1.0f; // Opacity

    // 거울이 아닌 물체들의 리스트 (for문으로 그리기 위함)
    vector<shared_ptr<Model>> m_basicList;
    vector<shared_ptr<Model>> m_tessellatedQuadList;
    vector<shared_ptr<Model>> m_tessellatedTriangleList;
    vector<shared_ptr<Model>> m_grassList;

};

} // namespace hlab
