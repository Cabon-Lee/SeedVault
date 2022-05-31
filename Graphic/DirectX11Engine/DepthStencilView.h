#pragma once

#include "RenderTargetBase.h"

class DepthStencilView : public RenderTargetBase
{

public:
	DepthStencilView();
	~DepthStencilView();

	HRESULT Initialize(
		ID3D11Device* pDevice,
		UINT width,
		UINT height,
		UINT bindFlags,
		DXGI_FORMAT textureFormat,
		DXGI_FORMAT shaderResourceFormat,
		DXGI_FORMAT depthStencilViewFormat);

	HRESULT CreateSharderResourceView(ID3D11Device* pDevice, DXGI_FORMAT shaderResourceFormat);

	void Reset();
	//---------------------------------------------------------------------------------------
	// µª½º ½ºÅÙ½Ç ºä
	//---------------------------------------------------------------------------------------
	ID3D11DepthStencilView* GetDepthSetncilView();

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStancilView;


private:
	HRESULT CreateDepthStencil(ID3D11Device* pDevice, DXGI_FORMAT depthStencilViewFormat);

};

