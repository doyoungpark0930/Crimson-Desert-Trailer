#include "ExampleApp.h"

#include <DirectXCollision.h> // 구와 광선 충돌 계산에 사용
#include <directxtk/DDSTextureLoader.h>
#include <directxtk/SimpleMath.h>
#include <tuple>
#include <vector>

#include "GeometryGenerator.h"
#include "GraphicsCommon.h"

namespace hlab {

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

ExampleApp::ExampleApp() : AppBase() {}

bool ExampleApp::Initialize() {

    if (!AppBase::Initialize())
        return false;

    AppBase::InitCubemaps(L"../Assets/Textures/Cubemaps/HDRI/mountain1/",
                          L"mountain1EnvHDR.dds", L"mountain1SpecularHDR.dds",
                          L"mountain1DiffuseHDR.dds", L"mountain1Brdf.dds");

    // 후처리용 화면 사각형
    {
        MeshData meshData = GeometryGenerator::MakeSquare();
        m_screenSquare =
            make_shared<Model>(m_device, m_context, vector{meshData});
    }

    // 환경 박스 및 배경
    {
        MeshData skyboxMesh = GeometryGenerator::MakeBox(250.0f);
        std::reverse(skyboxMesh.indices.begin(), skyboxMesh.indices.end());
        m_skybox = make_shared<Model>(m_device, m_context, vector{skyboxMesh});
    }

    // 테셀레이션 바닥
    {

        /* auto ground = GeometryGenerator::ReadFromFile(
            "../Assets/Textures/Objects/cliff_low/", "cliff_low.fbx",
            false,
            {10.0f, 10.0f});*/

        auto ground = GeometryGenerator::ReadFromFile(
            "../Assets/Textures/Objects/RockyDesert/", "RockyDesert_FBX.fbx",
            false, {5.0f, 5.0f});

        ground[0].albedoTextureFilename =
            "../Assets/Textures/PBR/TCom_Ground_Soil18_2.5x2.5_2K/"
            "TCom_Ground_Soil18_2.5x2.5_2K_albedo.tif";

        ground[0].normalTextureFilename =
            "../Assets/Textures/PBR/TCom_Ground_Soil18_2.5x2.5_2K/"
            "TCom_Ground_Soil18_2.5x2.5_2K_normal.tif";

        ground[0].heightTextureFilename =
            "../Assets/Textures/PBR/TCom_Ground_Soil18_2.5x2.5_2K/"
            "TCom_Ground_Soil18_2.5x2.5_2K_height.tif";

        ground[0].aoTextureFilename =
            "../Assets/Textures/PBR/TCom_Ground_Soil18_2.5x2.5_2K/"
            "TCom_Ground_Soil18_2.5x2.5_2K_ao.tif";

        ground[0].roughnessTextureFilename =
            "../Assets/Textures/PBR/TCom_Ground_Soil18_2.5x2.5_2K/"
            "TCom_Ground_Soil18_2.5x2.5_2K_roughness.tif";

        m_ground = make_shared<Model>(m_device, m_context, vector{ground});
        m_ground->m_materialConsts.GetCpu().invertNormalMapY =
            false; // GLTF는 true로
        m_ground->m_materialConsts.GetCpu().albedoFactor = Vector3(0.63f);
        m_ground->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
        m_ground->m_materialConsts.GetCpu().metallicFactor = 0.0f;
        m_ground->m_materialConsts.GetCpu().roughnessFactor = 1.0f;

        m_ground->scale = 50.0f;
        m_ground->position = Vector3(0.0f, 0.0f, 0.0f);
        m_ground->rotation.y = 1.15f;

        m_tessellatedTriangleList.push_back(m_ground);
        // m_basicList.push_back(m_ground); //노멀 벡터 보이게 테스트용
    }

    // Main Object
    {

        auto test1 = GeometryGenerator::ReadFromFile(
            "../Assets/Textures/Objects/cliff_low/", "cliff_low.fbx", false,
            {5.0f, 5.0f});
        test1[0].normalTextureFilename =
            "../Assets/Textures/PBR/TCom_Ground_Soil18_2.5x2.5_2K/"
            "TCom_Ground_Soil18_2.5x2.5_2K_normal.tif";
        /*
        test1[0].albedoTextureFilename =
            "../Assets/Textures/PBR/TCom_Ground_Soil18_2.5x2.5_2K/"
            "TCom_Ground_Soil18_2.5x2.5_2K_albedo.tif";

        test1[0].normalTextureFilename =
            "../Assets/Textures/PBR/TCom_Ground_Soil18_2.5x2.5_2K/"
            "TCom_Ground_Soil18_2.5x2.5_2K_normal.tif";

        test1[0].heightTextureFilename =
            "../Assets/Textures/PBR/TCom_Ground_Soil18_2.5x2.5_2K/"
            "TCom_Ground_Soil18_2.5x2.5_2K_height.tif";

        test1[0].aoTextureFilename =
            "../Assets/Textures/PBR/TCom_Ground_Soil18_2.5x2.5_2K/"
            "TCom_Ground_Soil18_2.5x2.5_2K_ao.tif";

        test1[0].roughnessTextureFilename =
            "../Assets/Textures/PBR/TCom_Ground_Soil18_2.5x2.5_2K/"
            "TCom_Ground_Soil18_2.5x2.5_2K_roughness.tif";*/

        m_testObj1 = make_shared<Model>(m_device, m_context, test1);
        m_testObj1->m_materialConsts.GetCpu().invertNormalMapY =
            false; // GLTF는 true로
        m_testObj1->m_materialConsts.GetCpu().albedoFactor = Vector3(0.63f);
        m_testObj1->m_materialConsts.GetCpu().emissionFactor = Vector3(0.0f);
        m_testObj1->m_materialConsts.GetCpu().metallicFactor = 0.0f;
        m_testObj1->m_materialConsts.GetCpu().roughnessFactor = 1.0f;
        m_testObj1->scale = 278.0f;
        m_testObj1->position = Vector3(-119.0f, 7.76f, -47.0f);
        m_testObj1->rotation.y = 1.2f;

        m_basicList.push_back(m_testObj1); // 리스트에 등록

        m_grass = make_shared<GrassModel>(m_device, m_context);
        m_grass->scale = 3.5f;
        m_grass->position = Vector3(10.5f, -2.1f, -18.9f);
        m_grass->rotation.y = 0.75f;
        m_basicGrass = m_grass;
    }

    // 조명 설정
    {
        // 조명 0은 고정
        m_globalConstsCPU.lights[0].radiance = Vector3(1.466f);
        m_globalConstsCPU.lights[0].position = Vector3(-15.0f, 5.7f, 15.5f);
        m_globalConstsCPU.lights[0].direction = Vector3(2.245f, -2.066f, -1.6f);
        m_globalConstsCPU.lights[0].spotPower = 1.185f;
        m_globalConstsCPU.lights[0].radius = 0.02f;
        m_globalConstsCPU.lights[0].type =
            LIGHT_SPOT | LIGHT_SHADOW; // Point with shadow
        /* 
        m_globalConstsCPU.lights[1].radiance = Vector3(5.0f);
        m_globalConstsCPU.lights[1].position = Vector3(11.0f, 1.0f, 5.0f);
        m_globalConstsCPU.lights[1].direction = Vector3(2.245f, -2.066f, -1.6f);
        m_globalConstsCPU.lights[1].spotPower = 1.5f;
        m_globalConstsCPU.lights[1].radius = 0.02f;
        m_globalConstsCPU.lights[1].type =
            LIGHT_SPOT | LIGHT_SHADOW; // Point with shadow
        */
    }

    // 조명 위치 표시
    {
        for (int i = 0; i < MAX_LIGHTS; i++) {
            MeshData sphere = GeometryGenerator::MakeSphere(1.0f, 20, 20);
            m_lightSphere[i] =
                make_shared<Model>(m_device, m_context, vector{sphere});
            m_lightSphere[i]->m_materialConsts.GetCpu().albedoFactor =
                Vector3(0.0f);
            m_lightSphere[i]->m_materialConsts.GetCpu().emissionFactor =
                Vector3(1.0f, 1.0f, 0.0f);
            m_lightSphere[i]->m_castShadow =
                false; // 조명 표시 물체들은 그림자 X

            if (m_globalConstsCPU.lights[i].type == 0)
                m_lightSphere[i]->m_isVisible = false;
            m_lightSphere[i]->position = m_globalConstsCPU.lights[i].position;
            m_basicList.push_back(m_lightSphere[i]); // 리스트에 등록
        }
    }

    return true;
}

void ExampleApp::UpdateLights(float dt) {

    // 그림자맵을 만들기 위한 시점
    for (int i = 0; i < MAX_LIGHTS; i++) {
        const auto &light = m_globalConstsCPU.lights[i];
        if (light.type & LIGHT_SHADOW) {

            Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
            if (abs(up.Dot(light.direction) + 1.0f) < 1e-5)
                up = Vector3(1.0f, 0.0f, 0.0f);

            // 그림자맵을 만들 때 필요
            Matrix lightViewRow = XMMatrixLookAtLH(
                light.position, light.position + light.direction, up);

            Matrix lightProjRow = XMMatrixPerspectiveFovLH(
                XMConvertToRadians(90.0f), 1.0f, 0.1f, 80.0f); // 원래 120

            m_shadowGlobalConstsCPU[i].eyeWorld = light.position;
            m_shadowGlobalConstsCPU[i].view = lightViewRow.Transpose();
            m_shadowGlobalConstsCPU[i].proj = lightProjRow.Transpose();
            m_shadowGlobalConstsCPU[i].invProj =
                lightProjRow.Invert().Transpose();
            m_shadowGlobalConstsCPU[i].viewProj =
                (lightViewRow * lightProjRow).Transpose();

            // LIGHT_FRUSTUM_WIDTH 확인
            // Vector4 eye(0.0f, 0.0f, 0.0f, 1.0f);
            // Vector4 xLeft(-1.0f, -1.0f, 0.0f, 1.0f);
            // Vector4 xRight(1.0f, 1.0f, 0.0f, 1.0f);
            // eye = Vector4::Transform(eye, lightProjRow);
            // xLeft = Vector4::Transform(xLeft, lightProjRow.Invert());
            // xRight = Vector4::Transform(xRight, lightProjRow.Invert());
            // xLeft /= xLeft.w;
            // xRight /= xRight.w;
            // cout << "LIGHT_FRUSTUM_WIDTH = " << xRight.x - xLeft.x << endl;

            D3D11Utils::UpdateBuffer(m_context, m_shadowGlobalConstsCPU[i],
                                     m_shadowGlobalConstsGPU[i]);

            // 그림자를 실제로 렌더링할 때 필요
            m_globalConstsCPU.lights[i].viewProj =
                m_shadowGlobalConstsCPU[i].viewProj;
            m_globalConstsCPU.lights[i].invProj =
                m_shadowGlobalConstsCPU[i].invProj;

            // 반사된 장면에서도 그림자를 그리고 싶다면 조명도 반사시켜서
            // 넣어주면 됩니다.
        }
    }
}

void ExampleApp::Update(float dt) {

    // 카메라의 이동
    m_camera.UpdateKeyboard(dt, m_keyPressed);

    // 반사 행렬 추가
    const Vector3 eyeWorld = m_camera.GetEyePos();
    const Matrix reflectRow;
    const Matrix viewRow = m_camera.GetViewRow();
    const Matrix projRow = m_camera.GetProjRow();

    UpdateLights(dt);

    // 공용 ConstantBuffer 업데이트
    AppBase::UpdateGlobalConstants(dt, eyeWorld, m_windStrength, viewRow,
                                   projRow, reflectRow);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        m_lightSphere[i]->position = m_globalConstsCPU.lights[i].position;
    }

    for (auto &i : m_basicList) {
        i->UpdateWorldRow(Matrix::CreateScale(i->scale) *
                          Matrix::CreateRotationY(3.1415f * i->rotation.y) *
                          Matrix::CreateTranslation(i->position));
    }
    for (auto &i : m_tessellatedTriangleList) {
        i->UpdateWorldRow(Matrix::CreateScale(i->scale) *
                          Matrix::CreateRotationY(3.1415f * i->rotation.y) *
                          Matrix::CreateTranslation(i->position));
    }
    if (m_basicGrass) {
        m_basicGrass->UpdateWorldRow(
            Matrix::CreateScale(m_basicGrass->scale) *
            Matrix::CreateRotationY(3.1415f * m_basicGrass->rotation.y) *
            Matrix::CreateTranslation(m_basicGrass->position));
        m_basicGrass->UpdateConstantBuffers(m_device, m_context);
    }

    for (auto &i : m_basicList) {
        i->UpdateConstantBuffers(m_device, m_context);
    }
    for (auto &i : m_tessellatedTriangleList) {
        i->UpdateConstantBuffers(m_device, m_context);
    }
}

void ExampleApp::Render() {

    AppBase::SetMainViewport();

    // 모든 샘플러들을 공통으로 사용 (뒤에서 더 추가됩니다.)
    m_context->VSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                             Graphics::sampleStates.data());
    m_context->PSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                             Graphics::sampleStates.data());
    m_context->DSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                             Graphics::sampleStates.data());

    // 공용 텍스춰들: "Common.hlsli"에서 register(t10)부터 시작
    vector<ID3D11ShaderResourceView *> commonSRVs = {   
        m_envSRV.Get(), m_specularSRV.Get(), m_irradianceSRV.Get(),
        m_brdfSRV.Get()}; 
    m_context->PSSetShaderResources(10, UINT(commonSRVs.size()),
                                    commonSRVs.data());

    const float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    vector<ID3D11RenderTargetView *> rtvs = {m_floatRTV.Get()};

    // 그림자맵 만들기
    AppBase::SetShadowViewport(); // 그림자맵 해상도
    AppBase::SetPipelineState(Graphics::depthOnlyPSO);
    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (m_globalConstsCPU.lights[i].type & LIGHT_SHADOW) {
            // RTS 생략 가능
            m_context->OMSetRenderTargets(0, NULL, m_shadowDSVs[i].Get());
            m_context->ClearDepthStencilView(m_shadowDSVs[i].Get(),
                                             D3D11_CLEAR_DEPTH, 1.0f, 0);

            AppBase::SetGlobalConsts(m_shadowGlobalConstsGPU[i]);
            for (auto &i : m_basicList)
                if (i->m_castShadow && i->m_isVisible)
                    i->Render(m_context);
            for (auto &i : m_tessellatedTriangleList)
                if (i->m_castShadow && i->m_isVisible)
                    i->TessellatedRender(m_context);
            // m_skybox->Render(m_context);
        }
    }

    // 다시 렌더링 해상도로 되돌리기
    AppBase::SetMainViewport();

    // 물체만 그리기
    for (size_t i = 0; i < rtvs.size(); i++) {
        m_context->ClearRenderTargetView(rtvs[i], clearColor);
    }
    m_context->OMSetRenderTargets(UINT(rtvs.size()), rtvs.data(),
                                  m_depthStencilView.Get());

    // 그림자맵들도 공용 텍스춰들 이후에 추가
    // 주의: 마지막 shadowDSV를 RenderTarget에서 해제한 후 설정
    vector<ID3D11ShaderResourceView *> shadowSRVs;
    for (int i = 0; i < MAX_LIGHTS; i++) {
        shadowSRVs.push_back(m_shadowSRVs[i].Get());
    }
    m_context->PSSetShaderResources(15, UINT(shadowSRVs.size()),
                                    shadowSRVs.data());

    m_context->ClearDepthStencilView(m_depthStencilView.Get(),
                                     D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                     1.0f, 0);
    AppBase::SetPipelineState(m_drawAsWire ? Graphics::defaultWirePSO
                                           : Graphics::defaultSolidPSO);
    AppBase::SetGlobalConsts(m_globalConstsGPU);

    for (auto &i : m_basicList) {
        i->Render(m_context);
    }

    AppBase::SetPipelineState(m_drawAsWire
                                  ? Graphics::tessellatedTriangleWirePSO
                                  : Graphics::tessellatedTriangleSolidPSO);

    for (auto &i : m_tessellatedTriangleList)
        i->TessellatedRender(m_context);

    AppBase::SetPipelineState(m_drawAsWire ? Graphics::grassWirePSO
                                           : Graphics::grassSolidPSO);
    if (m_basicGrass) {
        m_basicGrass->Render(m_context);
    }

    AppBase::SetPipelineState(Graphics::normalsPSO);
    for (auto &i : m_basicList) {
        if (i->m_drawNormals)
            i->RenderNormals(m_context);
    }

    AppBase::SetPipelineState(m_drawAsWire ? Graphics::skyboxWirePSO
                                           : Graphics::skyboxSolidPSO);

    m_skybox->Render(m_context);

    m_context->ResolveSubresource(m_resolvedBuffer.Get(), 0,
                                  m_floatBuffer.Get(), 0,
                                  DXGI_FORMAT_R16G16B16A16_FLOAT);

    // PostEffects
    AppBase::SetPipelineState(Graphics::postEffectsPSO);

    // 그림자맵 확인용 임시
    AppBase::SetGlobalConsts(m_shadowGlobalConstsGPU[0]);
    vector<ID3D11ShaderResourceView *> postEffectsSRVs = {
        m_resolvedSRV.Get(), m_shadowSRVs[0].Get()};

    // 20번에 넣어줌
    m_context->PSSetShaderResources(20, UINT(postEffectsSRVs.size()),
                                    postEffectsSRVs.data());
    m_context->OMSetRenderTargets(1, m_postEffectsRTV.GetAddressOf(), NULL);
    m_context->PSSetConstantBuffers(3, 1,
                                    m_postEffectsConstsGPU.GetAddressOf());
    m_screenSquare->Render(m_context);

    // 단순 이미지 처리와 블룸
    AppBase::SetPipelineState(Graphics::postProcessingPSO);
    m_postProcess.Render(m_context);
}

void ExampleApp::UpdateGUI() {

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("General")) {
        ImGui::Checkbox("Use FPV", &m_camera.m_useFirstPersonView);
        ImGui::Checkbox("Wireframe", &m_drawAsWire);
        if (ImGui::Checkbox("MSAA ON", &m_useMSAA)) {
            CreateBuffers();
        }
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Skybox")) {
        ImGui::SliderFloat("Strength", &m_globalConstsCPU.strengthIBL, 0.0f,
                           1.0f);
        ImGui::RadioButton("Env", &m_globalConstsCPU.textureToDraw, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Specular", &m_globalConstsCPU.textureToDraw, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Irradiance", &m_globalConstsCPU.textureToDraw, 2);
        ImGui::SliderFloat("EnvLodBias", &m_globalConstsCPU.envLodBias, 0.0f,
                           10.0f);
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Post Effects")) {
        int flag = 0;
        flag += ImGui::RadioButton("Render", &m_postEffectsConstsCPU.mode, 1);
        ImGui::SameLine();
        flag += ImGui::RadioButton("Depth", &m_postEffectsConstsCPU.mode, 2);
        flag += ImGui::SliderFloat(
            "DepthScale", &m_postEffectsConstsCPU.depthScale, 0.0, 0.1);
        flag += ImGui::SliderFloat("Fog", &m_postEffectsConstsCPU.fogStrength,
                                   0.0, 10.0);

        if (flag)
            D3D11Utils::UpdateBuffer(m_context, m_postEffectsConstsCPU,
                                     m_postEffectsConstsGPU);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Post Processing")) {
        int flag = 0;
        flag += ImGui::SliderFloat(
            "Bloom Strength",
            &m_postProcess.m_combineFilter.m_constData.strength, 0.0f, 1.0f);
        flag += ImGui::SliderFloat(
            "Exposure", &m_postProcess.m_combineFilter.m_constData.option1,
            0.0f, 10.0f);
        flag += ImGui::SliderFloat(
            "Gamma", &m_postProcess.m_combineFilter.m_constData.option2, 0.1f,
            5.0f);
        // 편의상 사용자 입력이 인식되면 바로 GPU 버퍼를 업데이트
        if (flag) {
            m_postProcess.m_combineFilter.UpdateConstantBuffers(m_device,
                                                                m_context);
        }
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Light1")) {
        // ImGui::SliderFloat3("Position",
        // &m_globalConstsCPU.lights[0].position.x,
        //                     -5.0f, 5.0f);
        ImGui::SliderFloat("Radius", &m_globalConstsCPU.lights[0].radius, 0.0f,
                           1.0f);
        ImGui::SliderFloat("PositionX", &m_globalConstsCPU.lights[0].position.x,
                           -200.0f, 200.0f);
        ImGui::SliderFloat("PositionY", &m_globalConstsCPU.lights[0].position.y,
                           -10.0f, 50.0f);
        ImGui::SliderFloat("PositionZ", &m_globalConstsCPU.lights[0].position.z,
                           -80.0f, 100.0f);
        ImGui::SliderFloat("DirectionX",
                           &m_globalConstsCPU.lights[0].direction.x, -20.0f,
                           10.0f);
        ImGui::SliderFloat("DirectionY",
                           &m_globalConstsCPU.lights[0].direction.y, -10.0f,
                           10.0f);
        ImGui::SliderFloat("DirectionZ",
                           &m_globalConstsCPU.lights[0].direction.z, -10.0f,
                           10.0f);
        ImGui::SliderFloat("SpotPower", &m_globalConstsCPU.lights[0].spotPower,
                           0.0f, 10.0f);
        ImGui::SliderFloat("LightLadiance",
                           &m_globalConstsCPU.lights[0].radiance.x, 0.0f,
                           10.0f);
        m_globalConstsCPU.lights[0].radiance.y =
            m_globalConstsCPU.lights[0].radiance.x;
        m_globalConstsCPU.lights[0].radiance.z =
            m_globalConstsCPU.lights[0].radiance.x;
        ImGui::TreePop();
    }
    /*
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Light2")) {
        // ImGui::SliderFloat3("Position",
        // &m_globalConstsCPU.lights[0].position.x,
        //                     -5.0f, 5.0f);
        ImGui::SliderFloat("Radius", &m_globalConstsCPU.lights[1].radius, 0.0f,
                           1.0f);
        ImGui::SliderFloat("PositionX", &m_globalConstsCPU.lights[1].position.x,
                           -50.0f, 50.0f);
        ImGui::SliderFloat("PositionY", &m_globalConstsCPU.lights[1].position.y,
                           -10.0f, 15.0f);
        ImGui::SliderFloat("PositionZ", &m_globalConstsCPU.lights[1].position.z,
                           -20.0f, 60.0f);
        ImGui::SliderFloat("DirectionX",
                           &m_globalConstsCPU.lights[1].direction.x, -20.0f,
                           10.0f);
        ImGui::SliderFloat("DirectionY",
                           &m_globalConstsCPU.lights[1].direction.y, -10.0f,
                           10.0f);
        ImGui::SliderFloat("DirectionZ",
                           &m_globalConstsCPU.lights[1].direction.z, -10.0f,
                           10.0f);
        ImGui::SliderFloat("SpotPower", &m_globalConstsCPU.lights[1].spotPower,
                           0.0f, 10.0f);
        ImGui::TreePop();
    }*/


    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Test1")) {

        ImGui::SliderFloat("Scale", &m_testObj1->scale, 0.1f, 300.0f);
        ImGui::SliderFloat("PositionX", &m_testObj1->position.x, -400.0f,
                           400.0f);
        ImGui::SliderFloat("PositionY", &m_testObj1->position.y, -50.0f, 20.0f);
        ImGui::SliderFloat("PositionZ", &m_testObj1->position.z, -200.0f,
                           200.0f);
              
        ImGui::SliderFloat("RotationY", &m_testObj1->rotation.y, 0.0f, 3.0f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Grass")) {

        ImGui::SliderFloat("Wind", &m_windStrength, -3.0f, 3.0f);
        ImGui::SliderFloat("Scale", &m_grass->scale, 0.1f, 30.0f);
        ImGui::SliderFloat("PositionX", &m_grass->position.x, -20.0f,
                           20.0f);
        ImGui::SliderFloat("PositionY", &m_grass->position.y, -10.0f, 10.0f);
        ImGui::SliderFloat("PositionZ", &m_grass->position.z, -25.0f,
                           25.0f);

        ImGui::SliderFloat("RotationY", &m_grass->rotation.y, 0.0f, 3.0f);
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Material")) {
        ImGui::SliderFloat("LodBias", &m_globalConstsCPU.lodBias, 0.0f, 10.0f);

        int flag = 0;

        flag += ImGui::SliderFloat(
            "Albedo", &m_ground->m_materialConsts.GetCpu().albedoFactor.x, 0.0f,
            1.0f);
        m_ground->m_materialConsts.GetCpu().albedoFactor.y =
            m_ground->m_materialConsts.GetCpu().albedoFactor.x;
        m_ground->m_materialConsts.GetCpu().albedoFactor.z =
            m_ground->m_materialConsts.GetCpu().albedoFactor.x;
        flag += ImGui::SliderFloat(
            "Metallic", &m_ground->m_materialConsts.GetCpu().metallicFactor,
            0.0f, 1.0f);
        m_testObj1->m_materialConsts.GetCpu().metallicFactor =
            m_ground->m_materialConsts.GetCpu().metallicFactor;
        flag += ImGui::SliderFloat(
            "Roughness", &m_ground->m_materialConsts.GetCpu().roughnessFactor,
            0.0f, 1.0f);
        flag += ImGui::SliderFloat(
            "Ao", &m_ground->m_materialConsts.GetCpu().aoFactor, 0.0f, 1.0f);
        flag += ImGui::CheckboxFlags(
            "AlbedoTexture", &m_ground->m_materialConsts.GetCpu().useAlbedoMap,
            1);
        flag += ImGui::CheckboxFlags(
            "EmissiveTexture",
            &m_ground->m_materialConsts.GetCpu().useEmissiveMap, 1);
        flag += ImGui::CheckboxFlags(
            "Use NormalMapping",
            &m_ground->m_materialConsts.GetCpu().useNormalMap, 1);
        flag += ImGui::CheckboxFlags(
            "Use AO", &m_ground->m_materialConsts.GetCpu().useAOMap, 1);
        flag += ImGui::CheckboxFlags(
            "Use HeightMapping", &m_ground->m_meshConsts.GetCpu().useHeightMap,
            1);
        flag += ImGui::SliderFloat("HeightScale",
                                   &m_ground->m_meshConsts.GetCpu().heightScale,
                                   0.0f, 0.5f);
        flag += ImGui::CheckboxFlags(
            "Use MetallicMap",
            &m_ground->m_materialConsts.GetCpu().useMetallicMap, 1);
        flag += ImGui::CheckboxFlags(
            "Use RoughnessMap",
            &m_ground->m_materialConsts.GetCpu().useRoughnessMap, 1);

        // if (flag) {
        //     m_mainObj->UpdateConstantBuffers(m_device, m_context);
        // }

        // ImGui::Checkbox("Draw Normals", &m_ground->m_drawNormals);

        ImGui::TreePop();
    }
}

} // namespace hlab
