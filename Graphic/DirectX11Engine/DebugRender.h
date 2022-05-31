#pragma once

#include <memory>
#include <vector>
#include "D3D11Define.h"

class RenderTargetView;
class DepthStencilView;

class Quad;
class VertexShader;
class PixelShader;

const unsigned int MAX_COUNT = 10;

class DebugRender
{
public:
	DebugRender();
	~DebugRender();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	void DrawDebugInfo(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::shared_ptr<RenderTargetView> pDebugInfo);

	void DrawDeferredDebug(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::vector<std::shared_ptr<RenderTargetView>> pDeferred,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader);

	void DrawDeferredDebug(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::vector<std::shared_ptr<RenderTargetView>> pDeferred,
		std::shared_ptr<DepthStencilView> pDepth,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader);

	void DrawDeferredDebug(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		const std::vector<std::unique_ptr<RenderTargetView>>& pDeferred,
		ID3D11ShaderResourceView* pRenderTarget,
		class VertexShader* pVertexShader,
		class PixelShader* pPixelShader);

private:
	// 화면 출력에 사용할 Quad
	std::shared_ptr<Quad> m_pScreenQaud;

	std::shared_ptr<VertexShader> m_DebugVertexShader;
	std::shared_ptr<PixelShader> m_DebugPixelShader;

};

