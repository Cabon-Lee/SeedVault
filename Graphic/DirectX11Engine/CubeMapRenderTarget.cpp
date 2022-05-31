#include "CubeMapRenderTarget.h"

#include "RenderTargetView.h"

CubeMapRenderTarget::CubeMapRenderTarget()
{

}

CubeMapRenderTarget::~CubeMapRenderTarget()
{

}

void CubeMapRenderTarget::Initialize(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice, unsigned int width, unsigned int height, unsigned int mipMaps)
{
	m_MipMaps = mipMaps;

	HRESULT _hr;

	try
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = mipMaps;
		texDesc.ArraySize = 6;
		texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;


		_hr = pDevice->CreateTexture2D(&texDesc, nullptr, m_pRTVTexture2D.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CubeMap CreateTexture2D Fail");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
		srvDesc.TextureCube.MostDetailedMip = 0;

		_hr = pDevice->CreateShaderResourceView(m_pRTVTexture2D.Get(), &srvDesc, m_pSharderResourceView.GetAddressOf());

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}

void CubeMapRenderTarget::CreateMipMap(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	std::vector<std::shared_ptr<RenderTargetView>>& pRenderTargets,
	unsigned int mipWidth, unsigned int mipHeight, unsigned int mipSlice)
{
	D3D11_BOX _sourceRegion;
	for (int i = 0; i < 6; ++i)
	{
		_sourceRegion.left = 0;
		_sourceRegion.right = mipWidth;
		_sourceRegion.top = 0;
		_sourceRegion.bottom = mipHeight;
		_sourceRegion.front = 0;
		_sourceRegion.back = 1;

		pDeviceContext->CopySubresourceRegion(
			m_pRTVTexture2D.Get(), 
			D3D11CalcSubresource(mipSlice, i, m_MipMaps), 0, 0, 0, 
			pRenderTargets[i]->GetTexture2D().Get(), 0, &_sourceRegion);
	}
}

Microsoft::WRL::ComPtr<ID3D11Texture2D>& CubeMapRenderTarget::GetTexture2D()
{
	return m_pRTVTexture2D;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& CubeMapRenderTarget::GetShaderResourceViewRawptr()
{
	return m_pSharderResourceView;
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& CubeMapRenderTarget::GetRenderTargetView(unsigned int idx)
{
	return m_pRenderTargetView[idx];
}

Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& CubeMapRenderTarget::GetDepthStencilView()
{
	return m_pDepthStencilView;
}



void CubeMapRenderTarget::CreateViewPort(UINT width, UINT height)
{
	m_ViewPort.TopLeftX = 0.0f;
	m_ViewPort.TopLeftY = 0.0f;
	m_ViewPort.Width = static_cast<FLOAT>(width);
	m_ViewPort.Height = static_cast<FLOAT>(height);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
}
