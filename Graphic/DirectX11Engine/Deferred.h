#pragma once

/// 미리 렌더 타겟을 만들어두는게 아니라
/// 필요할 때마다 만들어서 넣을 수 있어야 할까?
/// 일단은 이전에 만들어놓은 버전을 그대로 가져다쓰겠다
/// 무지성으로 복붙하고 하나씩 바꿔가면서 어떤 일이 벌어지는지를 봐야겠다

/// 기존 스타일은 렌더 타겟에서 ShaderResourceView를 반환해서 그림을 그렸다
/// 굳이 그럴게 아니라 여기서 그림을 그리게하면 더 편하지 않을까?

#include "D3D11Define.h"

class RenderTargetBase;
class RenderTargetView;
class DepthStencilView;

class VertexShader;
class PixelShader;
class Quad;

// 디버그 레이어는 바깥에서 가져온다
// 인덱스는 0번에 해당한다
const unsigned int DEBUG_LAYER = 0;
const unsigned int DRAW_LAYER = 8;
const unsigned int MAX_LAYER = DRAW_LAYER + 1;

const float null[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
const float nullColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
const float ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
const float WhiteColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

enum class eDEFERRED_LAYER
{
	eALBEDO = 0,
	eNORMAL,
	ePOSITION,
	eMATERIAL,
	eAMBIENT,
	eSHADOW,
	eEMISSIVE,
	eID,
};

const unsigned int ALBEDO_MAP = static_cast<int>(eDEFERRED_LAYER::eALBEDO);
const unsigned int NORMAL_MAP = static_cast<int>(eDEFERRED_LAYER::eNORMAL);
const unsigned int POSITION_MAP = static_cast<int>(eDEFERRED_LAYER::ePOSITION);
const unsigned int MATERIAL_MAP = static_cast<int>(eDEFERRED_LAYER::eMATERIAL);
const unsigned int AMBIENT_MAP = static_cast<int>(eDEFERRED_LAYER::eAMBIENT);
const unsigned int SHADOW_MAP = static_cast<int>(eDEFERRED_LAYER::eSHADOW);
const unsigned int EMISSIVE_MAP = static_cast<int>(eDEFERRED_LAYER::eEMISSIVE);
const unsigned int ID_MAP = static_cast<int>(eDEFERRED_LAYER::eID);

class Deferred
{
public:
	Deferred();
	~Deferred();

	// 내부에서 들고 있지 않으므로 원시포인터를 인자로 받는다
	void Initialize(ID3D11Device* pDevice, UINT width, UINT height);
	void OnResize(ID3D11Device* pDevice, UINT width, UINT height);

	void BeginRender(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	void BeginRenderWithOutDepth(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);

	void BindAccumReveal(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);

	void UnbindRenderTargets(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);

	void RenderTargetClear(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	void DeferredRenderTargetClear(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);

	void CombineRenderTargetOpaque(
		bool debug,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		class VertexShader* pVertexShader,
		class PixelShader* pPixelShader,
		ID3D11ShaderResourceView* pTextrueLightMap);

	void CombineRenderTargetTransparent(
		bool debug,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		class VertexShader* pVertexShader,
		class PixelShader* pPixelShader,
		ID3D11ShaderResourceView* pTextrueLightMap);

	void CompositeFrameBuffers(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::shared_ptr<RenderTargetView> pSwapChainRenderTarget,
		class VertexShader* pVertexShader,
		class PixelShader* pPixelShader);

public:
	// Getter
	std::unique_ptr<DepthStencilView>& GetDepthBuffer();
	const std::vector< std::unique_ptr<RenderTargetView>>& GetDrawLayers();

private:
	void CreateDepthStencilState(ID3D11Device* pDevice);

private:
	// 기즈모, 그리드, imgui가 들어갈 디버기용 버퍼 버퍼
	//std::shared_ptr<RenderTargetView> m_pDebuggerBuffer;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
	std::unique_ptr<DepthStencilView> m_pDepthBuffer;

	// OIT용 렌더 타겟을 가지고 있는다
	std::unique_ptr<RenderTargetView> m_pOpaqueFrameBuffer;
	std::unique_ptr<RenderTargetView> m_pAccumulation;
	std::unique_ptr<RenderTargetView> m_pRevealage;
	std::shared_ptr<VertexShader> m_VertexShader;


	// 화면 출력에 사용할 Quad
	std::shared_ptr<Quad> m_pScreenQaud;

	std::vector<std::unique_ptr<RenderTargetView>> m_pDeferredRTV_V;

};

