#include "BungieBloom.h"

#include "Shaders.h"
#include "UAVRenderTargetView.h"

#include "ConstantBufferManager.h"
#include "ErrorLogger.h"
#include "COMException.h"

BungieBloom::BungieBloom()
{

}

BungieBloom::~BungieBloom()
{

}

void BungieBloom::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width; m_Height = height;

	CreateBuffers(pDevice);
	CreateShader(pDevice);
	CreateBaseShader(pDevice);
}

void BungieBloom::CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_pBloomCurvedUAV.Reset();
	m_pBloomCurvedSRV.Reset();

	m_pTempUAV.Reset();
	m_pTempSRV.Reset();

	try
	{
		HRESULT _hr;

		D3D11_TEXTURE2D_DESC _texDesc;
		_texDesc.Width = m_Width;
		_texDesc.Height = m_Height;
		_texDesc.MipLevels = 1;
		_texDesc.ArraySize = 1;
		_texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		_texDesc.SampleDesc.Count = 1;
		_texDesc.SampleDesc.Quality = 0;
		_texDesc.Usage = D3D11_USAGE_DEFAULT;
		_texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		_texDesc.CPUAccessFlags = 0;
		_texDesc.MiscFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	 _blurredTex;
		_hr = pDevice->CreateTexture2D(&_texDesc, 0, _blurredTex.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		_hr = pDevice->CreateShaderResourceView(_blurredTex.Get(), &srvDesc,
			m_pBloomCurvedSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		_hr = pDevice->CreateUnorderedAccessView(_blurredTex.Get(), &uavDesc,
			m_pBloomCurvedUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	 _tempTex;
		_hr = pDevice->CreateTexture2D(&_texDesc, 0, _tempTex.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateShaderResourceView(_tempTex.Get(), &srvDesc, m_pTempSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");


		_hr = pDevice->CreateUnorderedAccessView(_tempTex.Get(), &uavDesc, m_pTempUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}

void BungieBloom::OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width; m_Height = height;

	CreateBuffers(pDevice);
}

void BungieBloom::ApplyBoomCurve(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	UAVRenderTargetView* renderTarget)
{
	// 현재 화면 사이즈와 정확히 일치하는 UAV에 작성한다
	ID3D11ShaderResourceView* arrViews[1] = { renderTarget->GetShaderResourceViewRawptr() };
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pBloomCurvedUAV.Get() };

	UINT numGroupsX = (UINT)ceilf(renderTarget->GetViewPort().Width / 32);
	UINT numGroupsY = (UINT)ceilf(renderTarget->GetViewPort().Height / 32);

	ConstantBufferManager::GetCSBloomBuffer()->data.gThreshold = m_Threshold;
	ConstantBufferManager::GetCSBloomBuffer()->data.gIntensity = m_Intensity;
	ConstantBufferManager::GetCSBloomBuffer()->data.height = m_Height;
	ConstantBufferManager::GetCSBloomBuffer()->data.pad = m_AdjustFactor;
	ConstantBufferManager::GetCSBloomBuffer()->ApplyChanges();

	pDeviceContext->CSSetConstantBuffers(0, 1, ConstantBufferManager::GetCSBloomBuffer()->GetAddressOf());

	// 바깥에서 가져온 SRV를 참고하여 블룸 커브를 먼저 적용시킨다
	{
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_BloomCurve->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	// Unbind the input texture from the CS for good housekeeping.
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);

	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// 복사해서 inputUAV에 넣어주는 작업
	{
		// 자원 묶기
		arrViews[0] = { m_pBloomCurvedSRV.Get() };
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		arrUAVs[0] = { renderTarget->GetUnorderedAccessViewRawptr() };
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_Copy->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}


	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	pDeviceContext->CSSetShader(0, 0, 0);

}

void BungieBloom::ColorCalb(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, UAVRenderTargetView* pSource,
	UAVRenderTargetView* pTarget)
{
	ID3D11ShaderResourceView* arrViews[1] = { pSource->GetShaderResourceViewRawptr() };
	ID3D11UnorderedAccessView* arrUAVs[1] = { pTarget->GetUnorderedAccessViewRawptr() };

	UINT numGroupsX = (UINT)ceilf(static_cast<float>(m_Width) / 1024) + 1;

	pDeviceContext->CSSetConstantBuffers(0, 1, ConstantBufferManager::GetCSBloomBuffer()->GetAddressOf());
	{
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_ColorCalb->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, 1, 1);
	}

	// Unbind the input texture from the CS for good housekeeping.
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);

	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	pDeviceContext->CSSetShader(0, 0, 0);
}

void BungieBloom::UpScaleStack(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, UAVRenderTargetView* inputRTV, UAVRenderTargetView* outputRTV)
{
	// Blur를 먹인 RTV의 SRV와 한 단계 큰 사이즈의 RTV의 SRV를 가져와서
	// 컴퓨트 쉐이더로 넘겨준다
	ID3D11ShaderResourceView* arrViews[1] = { inputRTV->GetShaderResourceViewRawptr() };
	ID3D11UnorderedAccessView* arrUAVs[1] = { outputRTV->GetUnorderedAccessViewRawptr() };

	FLOAT _nowRTVWidth = outputRTV->GetViewPort().Width;
	FLOAT _nowRTVHeight = outputRTV->GetViewPort().Height;

	UINT numGroupsX = (UINT)ceilf(_nowRTVWidth / 3);
	UINT numGroupsY = (UINT)ceilf(_nowRTVHeight / 3);

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

void BungieBloom::ApplyFactor(const IRenderOption& option)
{
	m_BlurCount = option.blurCount;

	m_Threshold = option.bloomThreshold;
	m_Intensity = option.intensity;
	m_AdjustFactor = option.adjustFactor;
}

void BungieBloom::CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_BloomCurve = std::make_shared<ComputeShader>();
	m_BloomCurve->Initialize(pDevice, L"../Data/Shader/cs_bungieBloom.hlsl", "BloomCurve", "cs_5_0");

	m_Copy = std::make_shared<ComputeShader>();
	m_Copy->Initialize(pDevice, L"../Data/Shader/cs_bungieBloom.hlsl", "Copy", "cs_5_0");

	m_HorizontalBlur = std::make_shared<ComputeShader>();
	m_HorizontalBlur->Initialize(pDevice, L"../Data/Shader/cs_gaussianBlur.hlsl", "HorzBlurCS", "cs_5_0");

	m_VerticalBlur = std::make_shared<ComputeShader>();
	m_VerticalBlur->Initialize(pDevice, L"../Data/Shader/cs_gaussianBlur.hlsl", "VertBlurCS", "cs_5_0");

	m_ColorCalb = std::make_shared<ComputeShader>();
	m_ColorCalb->Initialize(pDevice, L"../Data/Shader/cs_bungieBloom.hlsl", "ColorAdjust", "cs_5_0");

}
