#pragma once

#include "D3D11Define.h"

class RenderTargetBase;
class RenderTargetView;
class DepthStencilView;

class Forward
{
public:
	Forward();
	~Forward();

	void Initialize(ID3D11Device* pDevice, UINT width, UINT height);
	void OnResize(ID3D11Device* pDevice, UINT width, UINT height);
	void SetViewports(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);

	RenderTargetView* GetRenderTarget();
	DepthStencilView* GetDepthStencil();

	std::shared_ptr<RenderTargetView> GetRenderTargetView();

	// OMSetRenderTargets
	// ClearRenderTargetView
	// ClearDepthStencilView
	// IASetPrimitiveTopology
	// RSSetState
	// OMSetDepthStencilState 
	// OMSetBlendState
	// PSSetSamplers
	void BeginRender(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);


public:
	std::shared_ptr<RenderTargetView> m_pRenderTarget;

private:
	std::shared_ptr<DepthStencilView> m_pDepthStencil;



};

