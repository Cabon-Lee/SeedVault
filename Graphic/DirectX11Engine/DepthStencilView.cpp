#include "DepthStencilView.h"


DepthStencilView::DepthStencilView()
	: m_pDepthStancilView(nullptr)
{

}

DepthStencilView::~DepthStencilView()
{

}


HRESULT DepthStencilView::Initialize(
	ID3D11Device* pDevice,
	UINT width,
	UINT height,
	UINT bindFlags,
	DXGI_FORMAT textureFormat,
	DXGI_FORMAT shaderResourceFormat,
	DXGI_FORMAT depthStencilViewFormat)
{
	HRESULT _hr;

	try
	{
		_hr = CreateResource(pDevice, width, height, bindFlags, textureFormat, shaderResourceFormat);

		COM_ERROR_IF_FAILED(_hr, "DepthStancilView에서 CommomInitialize 실패");

		_hr = CreateSharderResourceView(pDevice, shaderResourceFormat);

		COM_ERROR_IF_FAILED(_hr, "DepthStancilView, CreateSharderResourceView FAIL ");

		_hr = CreateDepthStencil(pDevice, depthStencilViewFormat);

		COM_ERROR_IF_FAILED(_hr, "DepthStancilView에서 CreateDepthStencil 실패");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

	return _hr;
}

HRESULT DepthStencilView::CreateSharderResourceView(ID3D11Device* pDevice, DXGI_FORMAT shaderResourceFormat)
{

	D3D11_SHADER_RESOURCE_VIEW_DESC _SRVDesc;
	_SRVDesc.Format = shaderResourceFormat;
	_SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	_SRVDesc.Texture2D.MostDetailedMip = 0;
	_SRVDesc.Texture2D.MipLevels = 1;

	return pDevice->CreateShaderResourceView(m_pRTVTexture2D.Get(), &_SRVDesc, m_pSharderResourceView.GetAddressOf());
}

void DepthStencilView::Reset()
{
	BaseReset();

	m_pDepthStancilView.Reset();
	//ReleaseCOM(m_pDepthStancilView);
}

HRESULT DepthStencilView::CreateDepthStencil(ID3D11Device* pDevice, DXGI_FORMAT depthStencilViewFormat)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC _dsvDesc;
	_dsvDesc.Flags = 0;
	_dsvDesc.Format = depthStencilViewFormat;
	_dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	_dsvDesc.Texture2D.MipSlice = 0;

	return pDevice->CreateDepthStencilView(m_pRTVTexture2D.Get(), &_dsvDesc, m_pDepthStancilView.GetAddressOf());
}

ID3D11DepthStencilView* DepthStencilView::GetDepthSetncilView()
{
	return m_pDepthStancilView.Get();
}
