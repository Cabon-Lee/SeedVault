#include "RenderTargetView.h"

RenderTargetView::RenderTargetView()
	: m_pRenderTargerView(nullptr)
{

}

RenderTargetView::~RenderTargetView()
{
	m_pRenderTargerView.Reset();
}

HRESULT RenderTargetView::Initialize(
	Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
	UINT width,
	UINT height,
	UINT bindFlags,
	DXGI_FORMAT textureFormat,
	DXGI_FORMAT shaderResourceFormat,
	DXGI_FORMAT spcificFormat)
{

	HRESULT _hr;

	try
	{
		_hr = CreateResource(pDevice, width, height, bindFlags, textureFormat, shaderResourceFormat);

		COM_ERROR_IF_FAILED(_hr, "RenderTargetView CreateResource Fail");

		_hr = CreateShaderResourceView(pDevice.Get(), shaderResourceFormat);

		COM_ERROR_IF_FAILED(_hr, "DepthStancilView, CreateSharderResourceView FAIL ");

		_hr = CreateRenderTargetView(pDevice.Get(), spcificFormat);

		COM_ERROR_IF_FAILED(_hr, "RenderTargetView CreateDepthStencil Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

	return _hr;
}

void RenderTargetView::Initialize(Microsoft::WRL::ComPtr <ID3D11Device> pDevice, UINT width, UINT height, UINT mipMaps)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	m_Width = width;
	m_Height = height;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	HRESULT _hr;

	try
	{
		// Setup the render target texture description.
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipMaps;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		// Create the render target texture.
		_hr = pDevice->CreateTexture2D(&textureDesc, nullptr, m_pRTVTexture2D.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "Create Textrue2D Fail");

		// Setup the description of the render target view.
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		// Create the render target view.
		_hr = pDevice->CreateRenderTargetView(m_pRTVTexture2D.Get(), &renderTargetViewDesc, m_pRenderTargerView.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "Create RenderTargetView Fail");

		// Setup the description of the shader resource view.
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		// Create the shader resource view.
		_hr = pDevice->CreateShaderResourceView(m_pRTVTexture2D.Get(), &shaderResourceViewDesc, m_pSharderResourceView.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "Create ShaderResourceView Fail");

		m_ViewPort.TopLeftX = 0;
		m_ViewPort.TopLeftY = 0;
		m_ViewPort.Width = static_cast<FLOAT>(width);
		m_ViewPort.Height = static_cast<FLOAT>(height);
		m_ViewPort.MinDepth = 0.0f;
		m_ViewPort.MaxDepth = 1.0f;
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

}

HRESULT RenderTargetView::Initialize(Microsoft::WRL::ComPtr <ID3D11Device> pDevice, UINT width, UINT height, UINT bindFlags, DXGI_FORMAT textureFormat)
{
	return Initialize(pDevice, width, height, bindFlags, textureFormat, textureFormat, textureFormat);
}

HRESULT RenderTargetView::InitializeEX(
	Microsoft::WRL::ComPtr <ID3D11Device> pDevice, 
	UINT width, UINT height, UINT bindFlags, 
	DXGI_FORMAT textureFormat, DXGI_FORMAT shaderResourceFormat, DXGI_FORMAT spcificFormat, 
	D3D11_USAGE usage, UINT cpuFlag)
{
	HRESULT _hr;

	try
	{
		_hr = CreateResource(pDevice, width, height, bindFlags, textureFormat, shaderResourceFormat, usage, cpuFlag);

		COM_ERROR_IF_FAILED(_hr, "RenderTargetView CreateResource Fail");

		_hr = CreateShaderResourceView(pDevice.Get(), shaderResourceFormat);

		COM_ERROR_IF_FAILED(_hr, "DepthStancilView, CreateSharderResourceView FAIL ");

		_hr = CreateRenderTargetView(pDevice.Get(), spcificFormat);

		COM_ERROR_IF_FAILED(_hr, "RenderTargetView CreateDepthStencil Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

	return _hr;
}

HRESULT RenderTargetView::InitializeEX(Microsoft::WRL::ComPtr <ID3D11Device> pDevice, UINT width, UINT height, UINT bindFlags, DXGI_FORMAT textureFormat, D3D11_USAGE usage, UINT cpuFlag)
{
	return InitializeEX(pDevice, width, height, bindFlags, textureFormat, textureFormat, textureFormat, usage, cpuFlag);
}

HRESULT RenderTargetView::CreateShaderResourceView(ID3D11Device* pDevice, DXGI_FORMAT shaderResourceFormat)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC _SRVDesc = {};

	_SRVDesc.Format = shaderResourceFormat;
	_SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	_SRVDesc.Texture2D.MostDetailedMip = 0;
	_SRVDesc.Texture2D.MipLevels = -1;

	return pDevice->CreateShaderResourceView(m_pRTVTexture2D.Get(), &_SRVDesc, m_pSharderResourceView.GetAddressOf());
}

void RenderTargetView::Reset()
{
	BaseReset();

	m_pRenderTargerView.Reset();
}

HRESULT RenderTargetView::CreateRTVFromID3D11Texture(ID3D11Device* pDevice, ID3D11Texture2D* texture2D)
{

	HRESULT _hr;

	m_pRTVTexture2D = texture2D;

	// 바깥에서 스왑체인 혹은 다른 텍스쳐 정보를 가져와서 이를 토대로 뷰포트를 만든다
	D3D11_TEXTURE2D_DESC _desc;
	m_pRTVTexture2D->GetDesc(&_desc);

	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = static_cast<FLOAT>(_desc.Width);
	m_ViewPort.Height = static_cast<FLOAT>(_desc.Height);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	if (m_pRenderTargerView == nullptr)
	{
		_hr = CreateRenderTargetView(pDevice, _desc.Format);
	}

	// nullptr이 아니라면 Release를 호출하는가?
	return pDevice->CreateRenderTargetView(m_pRTVTexture2D.Get(), 0, m_pRenderTargerView.GetAddressOf());
}

HRESULT RenderTargetView::CreateRenderTargetView(ID3D11Device* pDevice, DXGI_FORMAT renderTargetViewFormat)
{
	D3D11_RENDER_TARGET_VIEW_DESC _RTVDesc;
	_RTVDesc.Format = renderTargetViewFormat;
	_RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	_RTVDesc.Texture2D.MipSlice = 0;

	return pDevice->CreateRenderTargetView(m_pRTVTexture2D.Get(), &_RTVDesc, m_pRenderTargerView.GetAddressOf());
}


ID3D11RenderTargetView* RenderTargetView::GetRenderTargerViewRawptr()
{
	return m_pRenderTargerView.Get();
}

ID3D11RenderTargetView* const* RenderTargetView::GetRenderTargetViewAddressOf()
{
	return m_pRenderTargerView.GetAddressOf();
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView::GetRenderTargetView()
{
	return m_pRenderTargerView;
}
