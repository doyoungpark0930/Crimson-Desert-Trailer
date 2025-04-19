#include "GraphicsCommon.h"

namespace hlab {

namespace Graphics {

// Sampler States
ComPtr<ID3D11SamplerState> linearWrapSS;
ComPtr<ID3D11SamplerState> linearClampSS;
ComPtr<ID3D11SamplerState> shadowPointSS;
ComPtr<ID3D11SamplerState> shadowCompareSS;
vector<ID3D11SamplerState *> sampleStates;

// Rasterizer States
ComPtr<ID3D11RasterizerState> solidRS;
ComPtr<ID3D11RasterizerState> solidCCWRS;
ComPtr<ID3D11RasterizerState> wireRS;
ComPtr<ID3D11RasterizerState> wireCCWRS;
ComPtr<ID3D11RasterizerState> postProcessingRS;
ComPtr<ID3D11RasterizerState> solidBothRS; // front and back
ComPtr<ID3D11RasterizerState> wireBothRS;

// Depth Stencil States
ComPtr<ID3D11DepthStencilState> drawDSS;       // 일반적으로 그리기
ComPtr<ID3D11DepthStencilState> maskDSS;       // 스텐실버퍼에 표시
ComPtr<ID3D11DepthStencilState> drawMaskedDSS; // 스텐실 표시된 곳만

// Blend States
ComPtr<ID3D11BlendState> mirrorBS;

// Shaders
ComPtr<ID3D11VertexShader> basicVS;
ComPtr<ID3D11VertexShader> tessellatedQuadVS;
ComPtr<ID3D11VertexShader> tessellatedTriangleVS;
ComPtr<ID3D11VertexShader> skyboxVS;
ComPtr<ID3D11VertexShader> samplingVS;
ComPtr<ID3D11VertexShader> normalVS;
ComPtr<ID3D11VertexShader> depthOnlyVS;
ComPtr<ID3D11VertexShader> depthOnlyGrassVS;
ComPtr<ID3D11VertexShader> grassVS;

ComPtr<ID3D11PixelShader> basicPS;
ComPtr<ID3D11PixelShader> skyboxPS;
ComPtr<ID3D11PixelShader> combinePS;
ComPtr<ID3D11PixelShader> bloomDownPS;
ComPtr<ID3D11PixelShader> bloomUpPS;
ComPtr<ID3D11PixelShader> normalPS;
ComPtr<ID3D11PixelShader> depthOnlyPS;
ComPtr<ID3D11PixelShader> depthOnlyGrassPS;
ComPtr<ID3D11PixelShader> postEffectsPS;
ComPtr<ID3D11PixelShader> grassPS;


ComPtr<ID3D11GeometryShader> normalGS;

ComPtr<ID3D11HullShader> tessellatedQuadHS;
ComPtr<ID3D11HullShader> tessellatedTriangleHS;
ComPtr<ID3D11DomainShader> tessellatedQuadDS;
ComPtr<ID3D11DomainShader> tessellatedTriangleDS;

// Input Layouts
ComPtr<ID3D11InputLayout> basicIL;
ComPtr<ID3D11InputLayout> samplingIL;
ComPtr<ID3D11InputLayout> skyboxIL;
ComPtr<ID3D11InputLayout> postProcessingIL;
ComPtr<ID3D11InputLayout> grassIL;

// Graphics Pipeline States
GraphicsPSO defaultSolidPSO;
GraphicsPSO defaultWirePSO;
GraphicsPSO tessellatedQuadSolidPSO;
GraphicsPSO tessellatedQuadWirePSO;
GraphicsPSO tessellatedTriangleSolidPSO;
GraphicsPSO tessellatedTriangleWirePSO;
GraphicsPSO stencilMaskPSO;
GraphicsPSO reflectSolidPSO;
GraphicsPSO reflectWirePSO;
GraphicsPSO mirrorBlendSolidPSO;
GraphicsPSO mirrorBlendWirePSO;
GraphicsPSO skyboxSolidPSO;
GraphicsPSO skyboxWirePSO;
GraphicsPSO reflectSkyboxSolidPSO;
GraphicsPSO reflectSkyboxWirePSO;
GraphicsPSO normalsPSO;
GraphicsPSO depthOnlyPSO;
GraphicsPSO depthOnlyGrassPSO;
GraphicsPSO postEffectsPSO;
GraphicsPSO postProcessingPSO;
GraphicsPSO grassSolidPSO;
GraphicsPSO grassWirePSO;

} // namespace Graphics

void Graphics::InitCommonStates(ComPtr<ID3D11Device> &device) {

    InitShaders(device);
    InitSamplers(device);
    InitRasterizerStates(device);
    InitBlendStates(device);
    InitDepthStencilStates(device);
    InitPipelineStates(device);
}

void Graphics::InitSamplers(ComPtr<ID3D11Device> &device) {

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, linearWrapSS.GetAddressOf());
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    device->CreateSamplerState(&sampDesc, linearClampSS.GetAddressOf());

    // shadowPointSS
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.BorderColor[0] = 1.0f; // 큰 Z값
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    device->CreateSamplerState(&sampDesc, shadowPointSS.GetAddressOf());

    // shadowCompareSS, 쉐이더 안에서는 SamplerComparisonState
    // Filter = "_COMPARISON_" 주의
    // https://www.gamedev.net/forums/topic/670575-uploading-samplercomparisonstate-in-hlsl/
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.BorderColor[0] = 100.0f; // 큰 Z값
    sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    device->CreateSamplerState(&sampDesc, shadowCompareSS.GetAddressOf());

    // 샘플러 순서가 "Common.hlsli"에서와 일관성 있어야 함
    sampleStates.push_back(linearWrapSS.Get());
    sampleStates.push_back(linearClampSS.Get());
    sampleStates.push_back(shadowPointSS.Get());
    sampleStates.push_back(shadowCompareSS.Get());
}

void Graphics::InitRasterizerStates(ComPtr<ID3D11Device> &device) {

    // Rasterizer States
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true;
    rastDesc.MultisampleEnable = true;
    ThrowIfFailed(
        device->CreateRasterizerState(&rastDesc, solidRS.GetAddressOf()));

    // 거울에 반사되면 삼각형의 Winding이 바뀌기 때문에 CCW로 그려야함
    rastDesc.FrontCounterClockwise = true;
    ThrowIfFailed(
        device->CreateRasterizerState(&rastDesc, solidCCWRS.GetAddressOf()));

    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    ThrowIfFailed(
        device->CreateRasterizerState(&rastDesc, wireCCWRS.GetAddressOf()));

    rastDesc.FrontCounterClockwise = false;
    ThrowIfFailed(
        device->CreateRasterizerState(&rastDesc, wireRS.GetAddressOf()));

    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = false;
    ThrowIfFailed(device->CreateRasterizerState(
        &rastDesc, postProcessingRS.GetAddressOf()));

    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE; // 양면
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true;
    rastDesc.MultisampleEnable = true;
    ThrowIfFailed(
        device->CreateRasterizerState(&rastDesc, solidBothRS.GetAddressOf()));

    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME; // 양면, Wire
    ThrowIfFailed(
        device->CreateRasterizerState(&rastDesc, wireBothRS.GetAddressOf()));
}

void Graphics::InitBlendStates(ComPtr<ID3D11Device> &device) {

    // "이미 그려져있는 화면"과 어떻게 섞을지를 결정
    // Dest: 이미 그려져 있는 값들을 의미
    // Src: 픽셀 쉐이더가 계산한 값들을 의미 (여기서는 마지막 거울)

    D3D11_BLEND_DESC mirrorBlendDesc;
    ZeroMemory(&mirrorBlendDesc, sizeof(mirrorBlendDesc));
    mirrorBlendDesc.AlphaToCoverageEnable = true; // MSAA
    mirrorBlendDesc.IndependentBlendEnable = false;
    // 개별 RenderTarget에 대해서 설정 (최대 8개)
    mirrorBlendDesc.RenderTarget[0].BlendEnable = true;
    mirrorBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
    mirrorBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
    mirrorBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

    mirrorBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    mirrorBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    mirrorBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

    // 필요하면 RGBA 각각에 대해서도 조절 가능
    mirrorBlendDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_ALL;

    ThrowIfFailed(
        device->CreateBlendState(&mirrorBlendDesc, mirrorBS.GetAddressOf()));
}

void Graphics::InitDepthStencilStates(ComPtr<ID3D11Device> &device) {

    // D3D11_DEPTH_STENCIL_DESC 옵션 정리
    // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_depth_stencil_desc
    // StencilRead/WriteMask: 예) uint8 중 어떤 비트를 사용할지

    // D3D11_DEPTH_STENCILOP_DESC 옵션 정리
    // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_depth_stencilop_desc
    // StencilPassOp : 둘 다 pass일 때 할 일
    // StencilDepthFailOp : Stencil pass, Depth fail 일 때 할 일
    // StencilFailOp : 둘 다 fail 일 때 할 일

    // m_drawDSS: 기본 DSS
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = false; // Stencil 불필요
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    // 앞면에 대해서 어떻게 작동할지 설정
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // 뒷면에 대해 어떻게 작동할지 설정 (뒷면도 그릴 경우)
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ThrowIfFailed(
        device->CreateDepthStencilState(&dsDesc, drawDSS.GetAddressOf()));

    // Stencil에 1로 표기해주는 DSS
    dsDesc.DepthEnable = true; // 이미 그려진 물체 유지
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = true;    // Stencil 필수
    dsDesc.StencilReadMask = 0xFF;  // 모든 비트 다 사용
    dsDesc.StencilWriteMask = 0xFF; // 모든 비트 다 사용
    // 앞면에 대해서 어떻게 작동할지 설정
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ThrowIfFailed(
        device->CreateDepthStencilState(&dsDesc, maskDSS.GetAddressOf()));

    // Stencil에 1로 표기된 경우에"만" 그리는 DSS
    // DepthBuffer는 초기화된 상태로 가정
    // D3D11_COMPARISON_EQUAL 이미 1로 표기된 경우에만 그리기
    // OMSetDepthStencilState(..., 1); <- 여기의 1
    dsDesc.DepthEnable = true;   // 거울 속을 다시 그릴때 필요
    dsDesc.StencilEnable = true; // Stencil 사용
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // <- 주의
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    ThrowIfFailed(
        device->CreateDepthStencilState(&dsDesc, drawMaskedDSS.GetAddressOf()));
}

void Graphics::InitShaders(ComPtr<ID3D11Device> &device) {

    // Shaders, InputLayouts

    vector<D3D11_INPUT_ELEMENT_DESC> basicIEs = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    vector<D3D11_INPUT_ELEMENT_DESC> samplingIED = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    vector<D3D11_INPUT_ELEMENT_DESC> skyboxIE = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    vector<D3D11_INPUT_ELEMENT_DESC> grassIEs = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, // Slot 0, 0부터 시작
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
         D3D11_INPUT_PER_VERTEX_DATA, 0},

        // 행렬 하나는 4x4라서 Element 4개 사용 (쉐이더에서는 행렬 하나)
        {"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, // Slot 1, 0부터 시작
         D3D11_INPUT_PER_INSTANCE_DATA, 1}, // 마지막 1은 instance step
        {"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
         D3D11_INPUT_PER_INSTANCE_DATA, 1}, // 마지막 1은 instance step
        {"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32,
         D3D11_INPUT_PER_INSTANCE_DATA, 1}, // 마지막 1은 instance step
        {"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48,
         D3D11_INPUT_PER_INSTANCE_DATA, 1}, // 마지막 1은 instance step
        {"COLOR", 0, DXGI_FORMAT_R32_FLOAT, 1, 64, // windStrength
         D3D11_INPUT_PER_INSTANCE_DATA, 1}};

    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"BasicVS.hlsl",
                                                 basicIEs, basicVS, basicIL);
    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"TessellatedQuadVS.hlsl", basicIEs, tessellatedQuadVS, basicIL);

    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"TessellatedTriangleVS.hlsl", basicIEs, tessellatedTriangleVS,
        basicIL);

    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"NormalVS.hlsl",
                                                 basicIEs, normalVS, basicIL);
    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"SamplingVS.hlsl", samplingIED, samplingVS, samplingIL);
    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"SkyboxVS.hlsl",
                                                 skyboxIE, skyboxVS, skyboxIL);
    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"GrassVS.hlsl",
                                                 grassIEs, grassVS, grassIL);
    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"DepthOnlyVS.hlsl", basicIEs, depthOnlyVS, basicIL);
    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"DepthOnlyGrassVS.hlsl", grassIEs, depthOnlyGrassVS, grassIL);

    D3D11Utils::CreatePixelShader(device, L"BasicPS.hlsl", basicPS);
    D3D11Utils::CreatePixelShader(device, L"NormalPS.hlsl", normalPS);
    D3D11Utils::CreatePixelShader(device, L"SkyboxPS.hlsl", skyboxPS);
    D3D11Utils::CreatePixelShader(device, L"CombinePS.hlsl", combinePS);
    D3D11Utils::CreatePixelShader(device, L"BloomDownPS.hlsl", bloomDownPS);
    D3D11Utils::CreatePixelShader(device, L"BloomUpPS.hlsl", bloomUpPS);
    D3D11Utils::CreatePixelShader(device, L"DepthOnlyPS.hlsl", depthOnlyPS);
    D3D11Utils::CreatePixelShader(device, L"DepthOnlyGrassPS.hlsl", depthOnlyGrassPS);
    D3D11Utils::CreatePixelShader(device, L"PostEffectsPS.hlsl", postEffectsPS);
    D3D11Utils::CreatePixelShader(device, L"GrassPS.hlsl", grassPS);


    D3D11Utils::CreateGeometryShader(device, L"NormalGS.hlsl", normalGS);

    D3D11Utils::CreateHullShader(device, L"TessellatedQuadHS.hlsl",
                                 tessellatedQuadHS);
    D3D11Utils::CreateHullShader(device, L"TessellatedTriangleHS.hlsl",
                                 tessellatedTriangleHS);

    D3D11Utils::CreateDomainShader(device, L"TessellatedQuadDS.hlsl",
                                   tessellatedQuadDS);
    D3D11Utils::CreateDomainShader(device, L"TessellatedTriangleDS.hlsl",
                                   tessellatedTriangleDS);
}

void Graphics::InitPipelineStates(ComPtr<ID3D11Device> &device) {

    // defaultSolidPSO;
    defaultSolidPSO.m_vertexShader = basicVS;
    defaultSolidPSO.m_inputLayout = basicIL;
    defaultSolidPSO.m_pixelShader = basicPS;
    defaultSolidPSO.m_rasterizerState = solidRS;
    defaultSolidPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // defaultWirePSO
    defaultWirePSO = defaultSolidPSO;
    defaultWirePSO.m_rasterizerState = wireRS;

    // tessellatedQuadSolidPSO;
    tessellatedQuadSolidPSO.m_vertexShader = tessellatedQuadVS;
    tessellatedQuadSolidPSO.m_hullShader = tessellatedQuadHS;
    tessellatedQuadSolidPSO.m_domainShader = tessellatedQuadDS; 
    tessellatedQuadSolidPSO.m_inputLayout = basicIL;
    tessellatedQuadSolidPSO.m_pixelShader = basicPS; 
    tessellatedQuadSolidPSO.m_rasterizerState = solidRS;
    tessellatedQuadSolidPSO.m_primitiveTopology =
        D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

    tessellatedQuadWirePSO = tessellatedQuadSolidPSO;
    tessellatedQuadWirePSO.m_rasterizerState = wireRS;

    // tessellatedTriangleSolidPSO;
    tessellatedTriangleSolidPSO.m_vertexShader = tessellatedTriangleVS;
    tessellatedTriangleSolidPSO.m_hullShader = tessellatedTriangleHS;
    tessellatedTriangleSolidPSO.m_domainShader = tessellatedTriangleDS;
    tessellatedTriangleSolidPSO.m_inputLayout = basicIL;
    tessellatedTriangleSolidPSO.m_pixelShader = basicPS;
    tessellatedTriangleSolidPSO.m_rasterizerState = solidRS;
    tessellatedTriangleSolidPSO.m_primitiveTopology =
        D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;

    tessellatedTriangleWirePSO = tessellatedTriangleSolidPSO;
    tessellatedTriangleWirePSO.m_rasterizerState = wireRS;

    // stencilMarkPSO;
    stencilMaskPSO = defaultSolidPSO;
    stencilMaskPSO.m_depthStencilState = maskDSS;
    stencilMaskPSO.m_stencilRef = 1;
    stencilMaskPSO.m_vertexShader = depthOnlyVS;
    stencilMaskPSO.m_pixelShader = depthOnlyPS;

    // reflectSolidPSO: 반사되면 Winding 반대
    reflectSolidPSO = defaultSolidPSO;
    reflectSolidPSO.m_depthStencilState = drawMaskedDSS;
    reflectSolidPSO.m_rasterizerState = solidCCWRS; // 반시계
    reflectSolidPSO.m_stencilRef = 1;

    // reflectWirePSO: 반사되면 Winding 반대
    reflectWirePSO = reflectSolidPSO;
    reflectWirePSO.m_rasterizerState = wireCCWRS; // 반시계
    reflectWirePSO.m_stencilRef = 1;

    // mirrorBlendSolidPSO;
    mirrorBlendSolidPSO = defaultSolidPSO;
    mirrorBlendSolidPSO.m_blendState = mirrorBS;
    mirrorBlendSolidPSO.m_depthStencilState = drawMaskedDSS;
    mirrorBlendSolidPSO.m_stencilRef = 1;

    // mirrorBlendWirePSO;
    mirrorBlendWirePSO = defaultWirePSO;
    mirrorBlendWirePSO.m_blendState = mirrorBS;
    mirrorBlendWirePSO.m_depthStencilState = drawMaskedDSS;
    mirrorBlendWirePSO.m_stencilRef = 1;

    // skyboxSolidPSO
    skyboxSolidPSO = defaultSolidPSO;
    skyboxSolidPSO.m_vertexShader = skyboxVS;
    skyboxSolidPSO.m_pixelShader = skyboxPS;
    skyboxSolidPSO.m_inputLayout = skyboxIL;

    // skyboxWirePSO
    skyboxWirePSO = skyboxSolidPSO;
    skyboxWirePSO.m_rasterizerState = wireRS;

    // reflectSkyboxSolidPSO
    reflectSkyboxSolidPSO = skyboxSolidPSO;
    reflectSkyboxSolidPSO.m_depthStencilState = drawMaskedDSS;
    reflectSkyboxSolidPSO.m_rasterizerState = solidCCWRS; // 반시계
    reflectSkyboxSolidPSO.m_stencilRef = 1;

    // reflectSkyboxWirePSO
    reflectSkyboxWirePSO = reflectSkyboxSolidPSO;
    reflectSkyboxWirePSO.m_rasterizerState = wireCCWRS;
    reflectSkyboxWirePSO.m_stencilRef = 1;

    // normalsPSO
    normalsPSO = defaultSolidPSO;
    normalsPSO.m_vertexShader = normalVS;
    normalsPSO.m_geometryShader = normalGS;
    normalsPSO.m_pixelShader = normalPS;
    normalsPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;


    // grassSolidPSO
    grassSolidPSO = defaultSolidPSO;
    grassSolidPSO.m_vertexShader = grassVS;
    grassSolidPSO.m_pixelShader = grassPS;
    grassSolidPSO.m_inputLayout = grassIL;
    grassSolidPSO.m_rasterizerState = solidBothRS; // 양면
    grassSolidPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // grassWirePSO
    grassWirePSO = grassSolidPSO;
    grassWirePSO.m_rasterizerState = wireBothRS; // 양면


    // depthOnlyPSO
    depthOnlyPSO = defaultSolidPSO;
    depthOnlyPSO.m_vertexShader = depthOnlyVS;
    depthOnlyPSO.m_pixelShader = depthOnlyPS;

    depthOnlyGrassPSO = grassSolidPSO;
    depthOnlyGrassPSO.m_vertexShader = depthOnlyGrassVS;
    depthOnlyGrassPSO.m_pixelShader = depthOnlyGrassPS;


    // postEffectsPSO
    postEffectsPSO.m_vertexShader = samplingVS;
    postEffectsPSO.m_pixelShader = postEffectsPS;
    postEffectsPSO.m_inputLayout = samplingIL;
    postEffectsPSO.m_rasterizerState = postProcessingRS;

    // postProcessingPSO
    postProcessingPSO.m_vertexShader = samplingVS;
    postProcessingPSO.m_pixelShader = depthOnlyPS; // dummy
    postProcessingPSO.m_inputLayout = samplingIL;
    postProcessingPSO.m_rasterizerState = postProcessingRS;
}

} // namespace hlab
