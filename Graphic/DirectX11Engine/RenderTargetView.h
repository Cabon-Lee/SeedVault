#pragma once

#include "RenderTargetBase.h"

class RenderTargetView :
	public RenderTargetBase
{
public:

	RenderTargetView();
	virtual ~RenderTargetView();

	HRESULT Initialize(
		Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
		UINT width,
		UINT height,
		UINT bindFlags,
		DXGI_FORMAT textureFormat,
		DXGI_FORMAT shaderResourceFormat,
		DXGI_FORMAT spcificFormat);

	HRESULT Initialize(
		Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
		UINT width,
		UINT height,
		UINT bindFlags,
		DXGI_FORMAT textureFormat);

	HRESULT InitializeEX(
		Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
		UINT width,
		UINT height,
		UINT bindFlags,
		DXGI_FORMAT textureFormat,
		DXGI_FORMAT shaderResourceFormat,
		DXGI_FORMAT spcificFormat,
		D3D11_USAGE usage, 
		UINT cpuFlag);

	HRESULT InitializeEX(
		Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
		UINT width,
		UINT height,
		UINT bindFlags,
		DXGI_FORMAT textureFormat,
		D3D11_USAGE usage,
		UINT cpuFlag);


	void Initialize(Microsoft::WRL::ComPtr <ID3D11Device> pDevice, UINT width, UINT height, UINT mipMap);

	HRESULT CreateShaderResourceView(ID3D11Device* pDevice, DXGI_FORMAT shaderResourceFormat);

	void Reset();

	HRESULT CreateRTVFromID3D11Texture(ID3D11Device* pDevice, ID3D11Texture2D* texture2D);

	//---------------------------------------------------------------------------------------
	// ¿œπ› ∑ª¥ı ≈∏∞Ÿ
	//---------------------------------------------------------------------------------------
	ID3D11RenderTargetView* GetRenderTargerViewRawptr();
	ID3D11RenderTargetView* const* GetRenderTargetViewAddressOf();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView();

protected:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargerView;

private:
	HRESULT CreateRenderTargetView(ID3D11Device* pDevice, DXGI_FORMAT renderTargetViewFormat);


};

