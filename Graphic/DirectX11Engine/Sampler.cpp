#include "Sampler.h"

#include "Shaders.h"

#include "RasterizerState.h"
#include "COMException.h"
#include "ErrorLogger.h"

Sampler::Sampler()
{

}

Sampler::~Sampler()
{

}

void Sampler::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width; m_Height = height;
	CreateBuffer(pDevice);
	CreateShader(pDevice);
	CreateBaseShader(pDevice);
}

void Sampler::OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width; m_Height = height;
	CreateBuffer(pDevice);
}

void Sampler::DownSample(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, ID3D11ShaderResourceView* pSource)
{
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };

	ID3D11ShaderResourceView* arrViews[1] = { pSource };
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pFirst4x4UAV.Get() };

	UINT numGroupsX = (UINT)ceilf(static_cast<float>(m_Width) / 4);
	UINT numGroupsY = (UINT)ceilf(static_cast<float>(m_Height) / 4);

	{
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_DownSample4x4->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);


	numGroupsX = (UINT)ceilf(static_cast<float>(numGroupsX) / 6);
	numGroupsY = (UINT)ceilf(static_cast<float>(numGroupsY) / 6);

	{
		arrViews[0] = { m_pFirst4x4SRV.Get() };
		arrUAVs[0] = { m_pFirst6x6UAV.Get() };

		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_DownSample6x6->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	numGroupsX = (UINT)ceilf(static_cast<float>(numGroupsX) / 6);
	numGroupsY = (UINT)ceilf(static_cast<float>(numGroupsY) / 6);

	{
		arrViews[0] = { m_pFirst6x6SRV.Get() };
		arrUAVs[0] = { m_pSecond6x6UAV.Get() };

		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_DownSample6x6->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	pDeviceContext->CSSetShader(0, 0, 0);
}

void Sampler::DownSampleEX(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, ID3D11ShaderResourceView* pSource)
{
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };

	ID3D11ShaderResourceView* arrViews[1] = { pSource };
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pFirst4x4UAV.Get() };

	UINT numGroupsX = (UINT)ceilf(static_cast<float>(m_Width ) / 4);
	UINT numGroupsY = (UINT)ceilf(static_cast<float>(m_Height) / 4);

	{
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_DownSample4x4->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);


	numGroupsX = (UINT)ceilf(static_cast<float>(numGroupsX) / 6);
	numGroupsY = (UINT)ceilf(static_cast<float>(numGroupsY) / 6);

	{
		arrViews[0] = { m_pFirst4x4SRV.Get() };
		arrUAVs[0] = { m_pFirst6x6UAV.Get() };

		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_DownSample6x6->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	pDeviceContext->CSSetShader(0, 0, 0);
}

void Sampler::UpSample(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, ID3D11ShaderResourceView* pSource)
{
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };

	UINT numGroupsX = (UINT)ceilf(static_cast<float>(m_Width )/ (4 * 6 * 6));
	UINT numGroupsY = (UINT)ceilf(static_cast<float>(m_Height) / (4 * 6 * 6));

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// 업 샘플링을 이 클래스의 TempUAV에 그린다
	{
		ID3D11ShaderResourceView* arrViews[1] = { pSource };
		ID3D11UnorderedAccessView* arrUAVs[1] = { m_pUpSecond6x6UAV.Get() };

		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_UpSample6x6->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	numGroupsX = (UINT)ceilf(static_cast<float>(m_Width )/ (4 * 6));
	numGroupsY = (UINT)ceilf(static_cast<float>(m_Height) / (4 * 6));

	{
		ID3D11ShaderResourceView* arrViews[1] = { m_pUpSecond6x6SRV.Get() };
		ID3D11UnorderedAccessView* arrUAVs[1] = { m_pUpFirst6x6UAV.Get() };

		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_UpSample6x6->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	numGroupsX = (UINT)ceilf(static_cast<float>(m_Width )/ (4));
	numGroupsY = (UINT)ceilf(static_cast<float>(m_Height) / (4));

	{
		ID3D11ShaderResourceView* arrViews[1] = { m_pUpFirst6x6SRV.Get() };
		ID3D11UnorderedAccessView* arrUAVs[1] = { m_pUpFirst4x4UAV.Get() };

		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_UpSample4x4->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	pDeviceContext->CSSetShader(0, 0, 0);

}

void Sampler::UpSample(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	ID3D11ShaderResourceView* pSource, ID3D11UnorderedAccessView* pTarget, UINT denominator)
{
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };

	UINT numGroupsX = (UINT)ceilf(static_cast<float>(m_Width )/ denominator) + 1;
	UINT numGroupsY = (UINT)ceilf(static_cast<float>(m_Height) / denominator) + 1;

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// 업 샘플링을 이 클래스의 TempUAV에 그린다
	{
		ID3D11ShaderResourceView* arrViews[1] = { pSource };
		ID3D11UnorderedAccessView* arrUAVs[1] = { pTarget };

		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		if (denominator == 4 || denominator == 1)
			pDeviceContext->CSSetShader(m_UpSample4x4->GetComputeShader(), NULL, 0);
		else
			pDeviceContext->CSSetShader(m_UpSample6x6->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	pDeviceContext->CSSetShader(0, 0, 0);
}

void Sampler::BilinearSample(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, ID3D11ShaderResourceView* pSource, ID3D11UnorderedAccessView* pTarget)
{
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };

	UINT numGroupsX = (UINT)ceilf(static_cast<float>(m_Width )/ 4) + 1;
	UINT numGroupsY = (UINT)ceilf(static_cast<float>(m_Height) / 4) + 1;

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// 업 샘플링을 이 클래스의 TempUAV에 그린다
	{
		ID3D11ShaderResourceView* arrViews[1] = { pSource };
		ID3D11UnorderedAccessView* arrUAVs[1] = { pTarget };

		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_UpSampleBilinear->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	pDeviceContext->CSSetShader(0, 0, 0);
}

void Sampler::Merge(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	ID3D11ShaderResourceView* pBlured, ID3D11ShaderResourceView* pOrigin,
	ID3D11UnorderedAccessView* pTarget,
	UINT denominator)
{
	ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };

	UINT numGroupsX = (UINT)ceilf(static_cast<float>(m_Width )/ denominator);
	UINT numGroupsY = (UINT)ceilf(static_cast<float>(m_Height) / denominator);

	numGroupsX = (UINT)ceilf(static_cast<float>(numGroupsX) / 32) + 1;
	numGroupsY = (UINT)ceilf(static_cast<float>(numGroupsY) / 32) + 1;

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// 업 샘플링을 이 클래스의 TempUAV에 그린다
	{
		ID3D11ShaderResourceView* arrViews[2] = { pOrigin, pBlured };
		ID3D11UnorderedAccessView* arrUAVs[1] = { pTarget };

		pDeviceContext->CSSetShaderResources(0, 2, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_Merge->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 2, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	pDeviceContext->CSSetShader(0, 0, 0);

}

void Sampler::Merge(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	ID3D11ShaderResourceView* pBlured, ID3D11ShaderResourceView* pOrigin, ID3D11UnorderedAccessView* pTarget)
{
	ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };

	UINT numGroupsX = (UINT)ceilf(static_cast<float>(m_Width )/ 32) + 1;
	UINT numGroupsY = (UINT)ceilf(static_cast<float>(m_Height) / 32) + 1;

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// 업 샘플링을 이 클래스의 TempUAV에 그린다
	{
		ID3D11ShaderResourceView* arrViews[2] = { pOrigin, pBlured };
		ID3D11UnorderedAccessView* arrUAVs[1] = { pTarget };

		pDeviceContext->CSSetShaderResources(0, 2, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_Merge->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 2, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	pDeviceContext->CSSetShader(0, 0, 0);

}

void Sampler::PixelMerge(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	ID3D11ShaderResourceView* pBlured, ID3D11ShaderResourceView* pOrigin,
	ID3D11RenderTargetView* pRenderTarget)
{
	ID3D11RenderTargetView* rt[1] = { nullptr };
	rt[0] = pRenderTarget;
	const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	pDeviceContext->OMSetRenderTargets(1, rt, NULL);
	pDeviceContext->ClearRenderTargetView(pRenderTarget, ClearColor);

	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	pDeviceContext->PSSetShaderResources(0, 1, &pBlured);
	pDeviceContext->PSSetShaderResources(1, 1, &pOrigin);


	ID3D11SamplerState* arrSamplers[1] = { RasterizerState::GetLinearSamplerState() };
	pDeviceContext->PSSetSamplers(0, 1, arrSamplers);


	// Set the shaders
	pDeviceContext->VSSetShader(m_pFullScreenQuadVS->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(m_PixelMerge->GetPixelShader(), NULL, 0);

	pDeviceContext->Draw(4, 0);

	// Cleanup
	ID3D11ShaderResourceView* arrViews[1];
	{
		ZeroMemory(arrViews, sizeof(arrViews));
		pDeviceContext->PSSetShaderResources(0, 1, arrViews);
		pDeviceContext->PSSetShaderResources(1, 1, arrViews);
		pDeviceContext->VSSetShader(NULL, NULL, 0);
		pDeviceContext->PSSetShader(NULL, NULL, 0);
	}
}

void Sampler::UpSampleAndMerge(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	ID3D11ShaderResourceView* pUpsampleSource,
	ID3D11ShaderResourceView* pMergeSource,
	ID3D11UnorderedAccessView* pTarget,
	UINT demonator)
{
	ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };

	UINT numGroupsX = (UINT)ceilf(static_cast<float>(m_Width )/ demonator);
	UINT numGroupsY = (UINT)ceilf(static_cast<float>(m_Height) / demonator);

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	{
		ID3D11ShaderResourceView* arrViews[1] = { pUpsampleSource };
		ID3D11UnorderedAccessView* arrUAVs[1] = { m_pTempUAV.Get() };

		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		if (demonator == 4)
			pDeviceContext->CSSetShader(m_UpSample4x4->GetComputeShader(), NULL, 0);
		else
			pDeviceContext->CSSetShader(m_UpSample6x6->GetComputeShader(), NULL, 0);

		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	{
		ID3D11ShaderResourceView* arrViews[2] = { pMergeSource , m_pTempSRV.Get() };
		ID3D11UnorderedAccessView* arrUAVs[1] = { m_pMergeUAV.Get() };

		pDeviceContext->CSSetShaderResources(0, 2, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_Merge->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 2, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	{
		ID3D11ShaderResourceView* arrViews[1] = { m_pMergeSRV.Get() };
		ID3D11UnorderedAccessView* arrUAVs[1] = { pTarget };

		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_Copy->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	pDeviceContext->CSSetShader(0, 0, 0);
}


void Sampler::ApplyFactor(const IRenderOption& option)
{

}

void Sampler::CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_pTempUAV.Reset();
	m_pTempSRV.Reset();

	m_pMergeUAV.Reset();
	m_pMergeSRV.Reset();

	m_pFirst4x4UAV.Reset();
	m_pFirst4x4SRV.Reset();

	m_pFirst6x6UAV.Reset();
	m_pFirst6x6SRV.Reset();

	m_pSecond6x6UAV.Reset();
	m_pSecond6x6SRV.Reset();

	m_pUpFirst4x4UAV.Reset();
	m_pUpFirst4x4SRV.Reset();

	m_pUpFirst6x6UAV.Reset();
	m_pUpFirst6x6SRV.Reset();

	m_pUpSecond6x6UAV.Reset();
	m_pUpSecond6x6SRV.Reset();

	try
	{
		HRESULT _hr;

		D3D11_TEXTURE2D_DESC _blurredTexDesc;
		_blurredTexDesc.Width = m_Width;
		_blurredTexDesc.Height = m_Height;
		_blurredTexDesc.MipLevels = 1;
		_blurredTexDesc.ArraySize = 1;
		_blurredTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
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
		srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		_hr = pDevice->CreateShaderResourceView(_blurredTex.Get(), &srvDesc,
			m_pTempSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		_hr = pDevice->CreateUnorderedAccessView(_blurredTex.Get(), &uavDesc,
			m_pTempUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");


		Microsoft::WRL::ComPtr<ID3D11Texture2D>	 _merge;
		_hr = pDevice->CreateTexture2D(&_blurredTexDesc, 0, _merge.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateUnorderedAccessView(_merge.Get(), &uavDesc, m_pMergeUAV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

		_hr = pDevice->CreateShaderResourceView(_merge.Get(), &srvDesc, m_pMergeSRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		//_blurredTexDesc.Width = m_Width / 4;
		//_blurredTexDesc.Height = m_Height / 4;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	 _first4x4;
		_hr = pDevice->CreateTexture2D(&_blurredTexDesc, 0, _first4x4.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateUnorderedAccessView(_first4x4.Get(), &uavDesc, m_pFirst4x4UAV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

		_hr = pDevice->CreateShaderResourceView(_first4x4.Get(), &srvDesc, m_pFirst4x4SRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");


		//_blurredTexDesc.Width /= 6;
		//_blurredTexDesc.Height /= 6;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	 _first6x6;
		_hr = pDevice->CreateTexture2D(&_blurredTexDesc, 0, _first6x6.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateUnorderedAccessView(_first6x6.Get(), &uavDesc, m_pFirst6x6UAV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

		_hr = pDevice->CreateShaderResourceView(_first6x6.Get(), &srvDesc, m_pFirst6x6SRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		//_blurredTexDesc.Width /= 6;
		//_blurredTexDesc.Height /= 6;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	 _second6x6;
		_hr = pDevice->CreateTexture2D(&_blurredTexDesc, 0, _second6x6.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateUnorderedAccessView(_second6x6.Get(), &uavDesc, m_pSecond6x6UAV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

		_hr = pDevice->CreateShaderResourceView(_second6x6.Get(), &srvDesc, m_pSecond6x6SRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	 _UpFisrt4x4;
		_hr = pDevice->CreateTexture2D(&_blurredTexDesc, 0, _UpFisrt4x4.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateUnorderedAccessView(_UpFisrt4x4.Get(), &uavDesc, m_pUpFirst4x4UAV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

		_hr = pDevice->CreateShaderResourceView(_UpFisrt4x4.Get(), &srvDesc, m_pUpFirst4x4SRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	 _UpFirst6x6;
		_hr = pDevice->CreateTexture2D(&_blurredTexDesc, 0, _UpFirst6x6.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateUnorderedAccessView(_UpFirst6x6.Get(), &uavDesc, m_pUpFirst6x6UAV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

		_hr = pDevice->CreateShaderResourceView(_UpFirst6x6.Get(), &srvDesc, m_pUpFirst6x6SRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	 _UpSecond6x6;
		_hr = pDevice->CreateTexture2D(&_blurredTexDesc, 0, _UpSecond6x6.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D Fail");

		_hr = pDevice->CreateUnorderedAccessView(_UpSecond6x6.Get(), &uavDesc, m_pUpSecond6x6UAV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

		_hr = pDevice->CreateShaderResourceView(_UpSecond6x6.Get(), &srvDesc, m_pUpSecond6x6SRV.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}

void Sampler::CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_DownSample4x4 = std::make_shared<ComputeShader>();
	m_DownSample4x4->Initialize(pDevice, L"../Data/Shader/cs_bungieBloom.hlsl", "DownSample4x4", "cs_5_0");

	m_DownSample6x6 = std::make_shared<ComputeShader>();
	m_DownSample6x6->Initialize(pDevice, L"../Data/Shader/cs_bungieBloom.hlsl", "DownSample6x6", "cs_5_0");

	m_UpSample4x4 = std::make_shared<ComputeShader>();
	m_UpSample4x4->Initialize(pDevice, L"../Data/Shader/cs_bungieBloom.hlsl", "UpSample4x4", "cs_5_0");

	m_UpSample6x6 = std::make_shared<ComputeShader>();
	m_UpSample6x6->Initialize(pDevice, L"../Data/Shader/cs_bungieBloom.hlsl", "UpSample6x6", "cs_5_0");

	m_Merge = std::make_shared<ComputeShader>();
	m_Merge->Initialize(pDevice, L"../Data/Shader/cs_bungieBloom.hlsl", "Merge", "cs_5_0");

	m_Copy = std::make_shared<ComputeShader>();
	m_Copy->Initialize(pDevice, L"../Data/Shader/cs_bungieBloom.hlsl", "Copy", "cs_5_0");

	m_UpSampleBilinear = std::make_shared<ComputeShader>();
	m_UpSampleBilinear->Initialize(pDevice, L"../Data/Shader/cs_bungieBloom.hlsl", "UpSampleBilinearLerp4x4", "cs_5_0");

	m_PixelMerge = std::make_shared<PixelShader>();
	m_PixelMerge->Initialize(pDevice, L"../Data/Shader/ps_deferredTotal.hlsl", "Merge", "ps_5_0");




}
