#include "RenderTargetBase.h"

RenderTargetBase::RenderTargetBase() 
	: m_Width(0)
	, m_Height(0)
	, m_pRTVTexture2D(nullptr)
	, m_pSharderResourceView(nullptr)
	, m_ViewPort({0,})
{

}

RenderTargetBase::~RenderTargetBase()
{
	m_pRTVTexture2D.Reset();
	m_pSharderResourceView.Reset();
}

ID3D11ShaderResourceView* RenderTargetBase::GetShaderResourceViewRawptr()
{
	return m_pSharderResourceView.Get();
}

ID3D11ShaderResourceView* const* RenderTargetBase::GetShaderResourceViewAddressOf()
{
	return m_pSharderResourceView.GetAddressOf();
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderTargetBase::GetShaderResourceView()
{
	return m_pSharderResourceView;
}

Microsoft::WRL::ComPtr<ID3D11Texture2D>& RenderTargetBase::GetTexture2D()
{
	return m_pRTVTexture2D;
}

const D3D11_VIEWPORT& RenderTargetBase::GetViewPort()
{
	return m_ViewPort;
}

HRESULT RenderTargetBase::CreateResource(
	Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
	UINT width, 
	UINT height, 
	UINT bindFlags,
	DXGI_FORMAT textureFormat, 
	DXGI_FORMAT shaderResourceFormat,
	D3D11_USAGE usage,
	UINT cpuFlag)
{
	// 공통적으로 실행되는 초기화를 Base 클래스에서 해준다

	m_Width = width;
	m_Height = height;

	HRESULT _hr;

	try
	{
		_hr = CreateTexture2D(pDevice.Get(), textureFormat, bindFlags, usage, cpuFlag);

		COM_ERROR_IF_FAILED(_hr, "RenderTargetBase에서 CreateTexute2D 생성 실패");

	}
	catch(COMException& e)
	{
		ErrorLogger::Log(e);
	}

	CreateViewPort();

	return _hr;
}

void RenderTargetBase::BaseReset()
{
	m_pRTVTexture2D.Reset();
	m_pSharderResourceView.Reset();
	//ReleaseCOM(m_pRTVTexture2D);
	//ReleaseCOM(m_pSharderResourceView);
}

HRESULT RenderTargetBase::CreateTexture2D(ID3D11Device* pDevice, DXGI_FORMAT textureFormat, UINT bindFlags)
{
	D3D11_TEXTURE2D_DESC _textureDesc;
	_textureDesc.Width = static_cast<UINT>(m_Width);
	_textureDesc.Height = static_cast<UINT>(m_Height);
	_textureDesc.MipLevels = 1;
	_textureDesc.ArraySize = 1;
	_textureDesc.Format = textureFormat;
	_textureDesc.SampleDesc.Count = 1;
	_textureDesc.SampleDesc.Quality = 0;
	_textureDesc.Usage = D3D11_USAGE_DEFAULT;
	_textureDesc.BindFlags = bindFlags;
	_textureDesc.CPUAccessFlags = 0;
	_textureDesc.MiscFlags = 0;

	return pDevice->CreateTexture2D(&_textureDesc, 0, m_pRTVTexture2D.GetAddressOf());
}

HRESULT RenderTargetBase::CreateTexture2D(ID3D11Device* pDevice, DXGI_FORMAT textureFormat, UINT bindFlags, D3D11_USAGE usage, UINT cpuFlag)
{
	D3D11_TEXTURE2D_DESC _textureDesc;
	_textureDesc.Width = static_cast<UINT>(m_Width);
	_textureDesc.Height = static_cast<UINT>(m_Height);
	_textureDesc.MipLevels = 1;
	_textureDesc.ArraySize = 1;
	_textureDesc.Format = textureFormat;
	_textureDesc.SampleDesc.Count = 1;
	_textureDesc.SampleDesc.Quality = 0;
	_textureDesc.Usage = usage;
	_textureDesc.BindFlags = bindFlags;
	_textureDesc.CPUAccessFlags = cpuFlag;
	_textureDesc.MiscFlags = 0;

	return pDevice->CreateTexture2D(&_textureDesc, 0, m_pRTVTexture2D.GetAddressOf());
}

void RenderTargetBase::CreateViewPort()
{
	m_ViewPort.TopLeftX = 0.0f;
	m_ViewPort.TopLeftY = 0.0f;
	m_ViewPort.Width = static_cast<FLOAT>(m_Width);
	m_ViewPort.Height = static_cast<FLOAT>(m_Height);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
}
