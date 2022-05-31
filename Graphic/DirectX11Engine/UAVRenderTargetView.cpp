#include "UAVRenderTargetView.h"

UAVRenderTargetView::UAVRenderTargetView()
{

}

UAVRenderTargetView::~UAVRenderTargetView()
{

}

void UAVRenderTargetView::Initialize(
	Microsoft::WRL::ComPtr <ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width; m_Height = height;

	m_pRTVTexture2D.Reset();
	m_pRenderTargerView.Reset();
	m_pSharderResourceView.Reset();
	m_pUnorderedAccessView.Reset();

	try
	{
		HRESULT _hr;

		D3D11_TEXTURE2D_DESC texDesc;

		texDesc.Width = m_Width;
		texDesc.Height = m_Height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;


		_hr = pDevice->CreateTexture2D(&texDesc, 0, m_pRTVTexture2D.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateRenderTargetView(m_pRTVTexture2D.Get(), 0, m_pRenderTargerView.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateRenderTargetView Fail");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		_hr = pDevice->CreateShaderResourceView(m_pRTVTexture2D.Get(), &srvDesc, m_pSharderResourceView.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		/// Texutre2D이므로 D3D11_BUFFER_UAV에 대한 내용을 작성할 필요는 없다
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;	
		uavDesc.Texture2D.MipSlice = 0;
		_hr = pDevice->CreateUnorderedAccessView(m_pRTVTexture2D.Get(), &uavDesc, m_pUnorderedAccessView.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

	m_ViewPort.TopLeftX = 0.0f;
	m_ViewPort.TopLeftY = 0.0f;
	m_ViewPort.Width = static_cast<FLOAT>(m_Width);
	m_ViewPort.Height = static_cast<FLOAT>(m_Height);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
}

ID3D11UnorderedAccessView* UAVRenderTargetView::GetUnorderedAccessViewRawptr()
{
	return m_pUnorderedAccessView.Get();
}

ID3D11UnorderedAccessView* const* UAVRenderTargetView::GetUnorderedAccessViewAddressOf()
{
	return m_pUnorderedAccessView.GetAddressOf();
}

Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UAVRenderTargetView::GetUnorderedAccessView()
{
	return m_pUnorderedAccessView;
}
