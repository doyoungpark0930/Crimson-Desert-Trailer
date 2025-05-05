# 🔥 DirectX 11 Graphics Portfolio - 붉은 사막 트레일러 모작

<p align="center">
  <img src="https://github.com/user-attachments/assets/662b69d9-9f4e-419c-8732-aa3b77327447" width="800" />
</p>

🎞️ [동영상 보기 (YouTube)](https://www.youtube.com/watch?v=I2hCiYHD1lU)  
📘 [개인 공부 블로그 (Tistory)](https://pdy0930.tistory.com/)

---

## 🧪 프로젝트 개요

**펄어비스 붉은 사막 트레일러**의 한 장면을 모작한 DirectX 11 기반 그래픽스 포트폴리오입니다.  
실시간 렌더링 기술, 셰이더 개발, 볼륨 렌더링, 테셀레이션 등 다양한 기본 그래픽스 기술을 구현하였습니다.

## 🖥️ 테스트 환경 및 성능 참고

- CPU: Intel(R) Core(TM) i5-8265U (1.6GHz)  
- GPU: Intel UHD Graphics 620 (내장 그래픽)  
- RAM: 8GB

> ⚠️ 해당 포트폴리오는 **저사양 노트북 환경**에서 개발 및 테스트되었습니다.  
> 복잡한 연산이 많은 **볼륨 렌더링, 테셀레이션, 블룸 등의 구간에서 프레임 드랍이 심한 편**입니다.
> 개발은 저사양 환경에서도 구현 가능하도록 최적화를 고려해 진행하였습니다.

---

## 🛠️ 적용 기술

- **PBR (Physically Based Rendering)** 적용  
- **HDR + Tone Mapping**  
- **Tessellation**  
- **구름 볼륨 렌더링** (ShaderToy 기반 포팅)  
- **Perlin Noise 기반 잔디 배치 및 흔들림**  
- **그림자 기법**: PCF, PCSS  
- **MSAA (멀티샘플링) + Resolve 처리**  
- **감마 커렉션 고려 후처리**  
- **블룸 처리 시 업/다운 샘플링으로 anti-aliasing**  
- **텍스처 자동 포맷 판별 및 로딩 시스템**  

> 🔍 디버깅 도구: **RenderDoc**

---

## 📚 참고 자료 및 출처

### 1. 👨‍🏫 강의 및 기본 코드
- [홍정모 연구소 그래픽스 강의](https://www.honglab.ai/collections)

### 2. 📘 LearnOpenGL 참고
- [PBR 이론](https://learnopengl.com/PBR/Theory)  
- [감마 커렉션](https://learnopengl.com/Advanced-Lighting/Gamma-Correction)  
- [Peter Panning 문제 해결](https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping)

### 3. 🌩️ ShaderToy 예제 코드
- [Perlin Noise](https://www.shadertoy.com/view/3dVXDc)  
- [Volumetric Clouds](https://www.shadertoy.com/view/4ttSWf)
- [Cook-Torrance + Oren-Nayar - PBR 시도](https://www.shadertoy.com/view/MsSczh)


### 4. 🧬 기타 참고
- [Wave + Fractal Brownian Motion 이론](https://thebookofshaders.com/13/?lan=kr)  
- [C++ Perlin Noise 오픈소스](https://github.com/Reputeless/PerlinNoise)  
- [PBR 오픈소스 코드](https://github.com/Nadrin/PBR)  
- [MSAA 후처리 & HDR 처리](https://github.com/Microsoft/DirectXTK/wiki/Using-HDR-rendering)

---

### 📌 향후 구름 렌더링 개선 참고 자료

- ShaderToy 구름 렌더링 코드 제공자의 렌더링 해설 영상  
  👉 [YouTube - ShaderToy Volumetric Cloud Explanation](https://www.youtube.com/watch?v=BFld4EBO2RE)

- 구름의 그림자 및 지형 형성에 사용된 노이즈 이론  
  👉 [FBM(Fractional Brownian Motion) 설명](https://iquilezles.org/articles/fbm/)  
  👉 [Improved Noise 및 Terrain Shaping](https://iquilezles.org/articles/morenoise/)

- 구름을 좀 더 이해하기 위해 ShaderToy 및 Unreal Engine 소스를 참고하여  
  다양한 형태의 구름 예제 코드를 경험할 필요가 있음

---

## 🔭 향후 학습 및 발전 방향

- **3D Fluid Simulation 복습 및 응용 (연기 시뮬레이션)**  
  → 유한차분법, 편미분 방정식 해석, Implicit Integration 방식 등을 Compute Shader 기반으로 복습 및 응용

- **Curl Noise의 실제 활용 방식 분석 및 구현**

- **디퍼드 쉐이딩 (Deferred Shading)의 원리 이해 후 적용**

- **DirectX 12 기반으로 포팅**

