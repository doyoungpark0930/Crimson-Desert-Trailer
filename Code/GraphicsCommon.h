#pragma once

#include "D3D11Utils.h"
#include "GraphicsPSO.h"

namespace hlab {

// 참고: DirectX_Graphic-Samples 미니엔진
// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/GraphicsCommon.h

namespace Graphics {

// Samplers
extern ComPtr<ID3D11SamplerState> linearWrapSS;
extern ComPtr<ID3D11SamplerState> linearClampSS;
extern ComPtr<ID3D11SamplerState> shadowPointSS;
extern ComPtr<ID3D11SamplerState> shadowCompareSS;
extern vector<ID3D11SamplerState *> sampleStates;

// Rasterizer States
extern ComPtr<ID3D11RasterizerState> solidRS;
extern ComPtr<ID3D11RasterizerState> solidCCWRS; // Counter-ClockWise
extern ComPtr<ID3D11RasterizerState> wireRS;
extern ComPtr<ID3D11RasterizerState> wireCCWRS;
extern ComPtr<ID3D11RasterizerState> postProcessingRS;
extern ComPtr<ID3D11RasterizerState> solidBothRS; // front and back
extern ComPtr<ID3D11RasterizerState> wireBothRS;

// Depth Stencil States
extern ComPtr<ID3D11DepthStencilState> drawDSS; // 일반적으로 그리기
extern ComPtr<ID3D11DepthStencilState> maskDSS; // 스텐실버퍼에 표시
extern ComPtr<ID3D11DepthStencilState> drawMaskedDSS; // 스텐실 표시된 곳만

// Shaders
extern ComPtr<ID3D11VertexShader> basicVS;
extern ComPtr<ID3D11VertexShader> tessellatedQuadVS;
extern ComPtr<ID3D11VertexShader> tessellatedTriangleVS;
extern ComPtr<ID3D11VertexShader> skyboxVS; 
extern ComPtr<ID3D11VertexShader> samplingVS;
extern ComPtr<ID3D11VertexShader> normalVS;
extern ComPtr<ID3D11VertexShader> depthOnlyVS;
extern ComPtr<ID3D11VertexShader> grassVS;

extern ComPtr<ID3D11PixelShader> basicPS;
extern ComPtr<ID3D11PixelShader> skyboxPS;
extern ComPtr<ID3D11PixelShader> combinePS;
extern ComPtr<ID3D11PixelShader> bloomDownPS;
extern ComPtr<ID3D11PixelShader> bloomUpPS;
extern ComPtr<ID3D11PixelShader> normalPS;
extern ComPtr<ID3D11PixelShader> depthOnlyPS;
extern ComPtr<ID3D11PixelShader> postEffectsPS;
extern ComPtr<ID3D11PixelShader> grassPS;

extern ComPtr<ID3D11GeometryShader> normalGS;
extern ComPtr<ID3D11HullShader> tessellatedQuadHS;
extern ComPtr<ID3D11HullShader> tessellatedTriangleHS;
extern ComPtr<ID3D11DomainShader> tessellatedQuadDS;
extern ComPtr<ID3D11DomainShader> tessellatedTriangleDS;

// Input Layouts
extern ComPtr<ID3D11InputLayout> basicIL;
extern ComPtr<ID3D11InputLayout> samplingIL;
extern ComPtr<ID3D11InputLayout> skyboxIL;
extern ComPtr<ID3D11InputLayout> postProcessingIL;
extern ComPtr<ID3D11InputLayout> grassIL;

// Blend States
extern ComPtr<ID3D11BlendState> mirrorBS;

// Graphics Pipeline States
extern GraphicsPSO defaultSolidPSO;
extern GraphicsPSO defaultWirePSO;
extern GraphicsPSO tessellatedQuadSolidPSO;
extern GraphicsPSO tessellatedQuadWirePSO;
extern GraphicsPSO tessellatedTriangleSolidPSO;
extern GraphicsPSO tessellatedTriangleWirePSO;
extern GraphicsPSO stencilMaskPSO;
extern GraphicsPSO reflectSolidPSO;
extern GraphicsPSO reflectWirePSO;
extern GraphicsPSO mirrorBlendSolidPSO;
extern GraphicsPSO mirrorBlendWirePSO;
extern GraphicsPSO skyboxSolidPSO;
extern GraphicsPSO skyboxWirePSO;
extern GraphicsPSO reflectSkyboxSolidPSO;
extern GraphicsPSO reflectSkyboxWirePSO;
extern GraphicsPSO normalsPSO;
extern GraphicsPSO depthOnlyPSO;
extern GraphicsPSO postEffectsPSO;
extern GraphicsPSO postProcessingPSO;
extern GraphicsPSO grassSolidPSO;
extern GraphicsPSO grassWirePSO;

void InitCommonStates(ComPtr<ID3D11Device> &device);

// 내부적으로 InitCommonStates()에서 사용
void InitSamplers(ComPtr<ID3D11Device> &device);
void InitRasterizerStates(ComPtr<ID3D11Device> &device);
void InitBlendStates(ComPtr<ID3D11Device> &device);
void InitDepthStencilStates(ComPtr<ID3D11Device> &device);
void InitPipelineStates(ComPtr<ID3D11Device> &device);
void InitShaders(ComPtr<ID3D11Device> &device);

} // namespace Graphics

} // namespace hlab
