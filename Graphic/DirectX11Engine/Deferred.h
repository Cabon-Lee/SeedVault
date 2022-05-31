#pragma once

/// �̸� ���� Ÿ���� �����δ°� �ƴ϶�
/// �ʿ��� ������ ���� ���� �� �־�� �ұ�?
/// �ϴ��� ������ �������� ������ �״�� �����پ��ڴ�
/// ���������� �����ϰ� �ϳ��� �ٲ㰡�鼭 � ���� ������������ ���߰ڴ�

/// ���� ��Ÿ���� ���� Ÿ�ٿ��� ShaderResourceView�� ��ȯ�ؼ� �׸��� �׷ȴ�
/// ���� �׷��� �ƴ϶� ���⼭ �׸��� �׸����ϸ� �� ������ ������?

#include "D3D11Define.h"

class RenderTargetBase;
class RenderTargetView;
class DepthStencilView;

class VertexShader;
class PixelShader;
class Quad;

// ����� ���̾�� �ٱ����� �����´�
// �ε����� 0���� �ش��Ѵ�
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

	// ���ο��� ��� ���� �����Ƿ� ���������͸� ���ڷ� �޴´�
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
	// �����, �׸���, imgui�� �� ������ ���� ����
	//std::shared_ptr<RenderTargetView> m_pDebuggerBuffer;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
	std::unique_ptr<DepthStencilView> m_pDepthBuffer;

	// OIT�� ���� Ÿ���� ������ �ִ´�
	std::unique_ptr<RenderTargetView> m_pOpaqueFrameBuffer;
	std::unique_ptr<RenderTargetView> m_pAccumulation;
	std::unique_ptr<RenderTargetView> m_pRevealage;
	std::shared_ptr<VertexShader> m_VertexShader;


	// ȭ�� ��¿� ����� Quad
	std::shared_ptr<Quad> m_pScreenQaud;

	std::vector<std::unique_ptr<RenderTargetView>> m_pDeferredRTV_V;

};

