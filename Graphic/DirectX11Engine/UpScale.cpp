#include "UpScale.h"

#include "Shaders.h"

#include "RenderTargetView.h"
#include "UAVRenderTargetView.h"

#include "ErrorLogger.h"
#include "COMException.h"

UpScale::UpScale()
{

}

UpScale::~UpScale()
{

}

void UpScale::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width; m_Height = height;

	CreateBuffer(pDevice);
	CreateShader(pDevice);
	CreateBaseShader(pDevice);
}

void UpScale::OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width; m_Height = height;

	CreateBuffer(pDevice);
}

void UpScale::Apply(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	UAVRenderTargetView* inputRTV, UAVRenderTargetView* outputRTV)
{
	// Blur를 먹인 RTV의 SRV와 한 단계 큰 사이즈의 RTV의 SRV를 가져와서
	// 컴퓨트 쉐이더로 넘겨준다
	ID3D11ShaderResourceView* arrViews[1] =	{ inputRTV->GetShaderResourceViewRawptr()	};
	ID3D11UnorderedAccessView* arrUAVs[1] = { outputRTV->GetUnorderedAccessViewRawptr() };

	FLOAT _nowRTVWidth = outputRTV->GetViewPort().Width;
	FLOAT _nowRTVHeight = outputRTV->GetViewPort().Height;

	UINT numGroupsX = (UINT)ceilf(_nowRTVWidth / 32);
	UINT numGroupsY = (UINT)ceilf(_nowRTVHeight / 32);

	// 바깥에서 가져온 SRV를 참고하여 블룸 커브를 먼저 적용시킨다
	{
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_UpScalShader->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	// Unbind the input texture from the CS for good housekeeping.
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);

	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	pDeviceContext->CSSetShader(0, 0, 0);
}

void UpScale::ApplyFactor(const IRenderOption& option)
{

}

void UpScale::CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_pUpScaleUAV.Reset();
	m_pUpScaleSRV.Reset();

	try
	{
		HRESULT _hr;

		D3D11_TEXTURE2D_DESC _blurredTexDesc;
		_blurredTexDesc.Width = m_Width;
		_blurredTexDesc.Height = m_Height;
		_blurredTexDesc.MipLevels = 1;
		_blurredTexDesc.ArraySize = 1;
		_blurredTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		_blurredTexDesc.SampleDesc.Count = 1;
		_blurredTexDesc.SampleDesc.Quality = 0;
		_blurredTexDesc.Usage = D3D11_USAGE_DEFAULT;
		_blurredTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		_blurredTexDesc.CPUAccessFlags = 0;
		_blurredTexDesc.MiscFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	 _blurredTex;
		_hr = pDevice->CreateTexture2D(&_blurredTexDesc, 0, _blurredTex.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		_hr = pDevice->CreateShaderResourceView(_blurredTex.Get(), &srvDesc, m_pUpScaleSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		_hr = pDevice->CreateUnorderedAccessView(_blurredTex.Get(), &uavDesc, m_pUpScaleUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");


	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);

	}
}

void UpScale::CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_UpScalShader = std::make_shared<ComputeShader>();
	m_UpScalShader->Initialize(pDevice, L"../Data/Shader/cs_bungieBloom.hlsl", "BilinearInterporation", "cs_5_0");

	m_Copy = std::make_shared<ComputeShader>();
	m_Copy->Initialize(pDevice, L"../Data/Shader/cs_bungieBloom.hlsl", "Copy", "cs_5_0");
}
