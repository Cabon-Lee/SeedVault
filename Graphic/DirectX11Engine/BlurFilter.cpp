#include "BlurFilter.h"

#include "Shaders.h"
#include "RenderTargetView.h"
#include "UAVRenderTargetView.h"

#include "ConstantBufferManager.h"

#include "COMException.h"
#include "ErrorLogger.h"

BlurFilter::BlurFilter()
	: m_GaussianWeight(5)
{

}

BlurFilter::~BlurFilter()
{

}

void BlurFilter::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_pBlurredOutputSRV.Reset();
	m_pBlurredOutputUAV.Reset();

	m_Width = width; m_Height = height;

	try
	{
		HRESULT _hr;

		D3D11_TEXTURE2D_DESC _blurredTexDesc;
		_blurredTexDesc.Width = width;
		_blurredTexDesc.Height = height;
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
		_hr = pDevice->CreateShaderResourceView(_blurredTex.Get(), &srvDesc, m_pBlurredOutputSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		_hr = pDevice->CreateUnorderedAccessView(_blurredTex.Get(), &uavDesc, m_pBlurredOutputUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");


	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);

	}

	CreateShader(pDevice);
	CreateBaseShader(pDevice);
}

void BlurFilter::CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_HorizontalBlur = std::make_shared<ComputeShader>();
	m_HorizontalBlur->Initialize(pDevice, L"../Data/Shader/cs_gaussianBlur.hlsl", "HorzBlurCS", "cs_5_0");

	m_VerticalBlur = std::make_shared<ComputeShader>();
	m_VerticalBlur->Initialize(pDevice, L"../Data/Shader/cs_gaussianBlur.hlsl", "VertBlurCS", "cs_5_0");
}

void BlurFilter::OnResize(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	Initialize(pDevice, width, height);
}

void BlurFilter::Apply(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputSRV,
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> inputUAV)
{
	for (int i = 0; i < m_BlurCount; ++i)
	{
		// 쉐이더 자원 묶기
		// Input
		ID3D11ShaderResourceView* arrViews[1] = { inputSRV.Get() };
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);

		// Output
		ID3D11UnorderedAccessView* arrUAVs[1] = { m_pBlurredOutputUAV.Get() };
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		// HORIZONTAL blur pass.
		pDeviceContext->CSSetShader(m_HorizontalBlur->GetComputeShader(), NULL, 0);

		// How many groups do we need to dispatch to cover a row of pixels, where each
		// group covers 256 pixels (the 256 is defined in the ComputeShader).
		// m_Width 화면 사이즈가 아니라 마지막으로 샘플링한 렌더 타겟의 사이즈이다
		UINT numGroupsX = (UINT)ceilf(m_Width / 256.0f);
		pDeviceContext->Dispatch(numGroupsX, m_Width, 1);

		// Unbind the input texture from the CS for good housekeeping.
		ID3D11ShaderResourceView* nullSRV[1] = { 0 };
		pDeviceContext->CSSetShaderResources(0, 1, nullSRV);

		// Unbind output from compute shader (we are going to use this output as an input in the next pass, 
		// and a resource cannot be both an output and input at the same time.
		ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);


		// 쉐이더 자원 묶기
		arrViews[0] = { m_pBlurredOutputSRV.Get() };
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);

		arrUAVs[0] = { inputUAV.Get() };
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		// VERTICAL blur pass.
		// How many groups do we need to dispatch to cover a column of pixels, where each
		// group covers 256 pixels  (the 256 is defined in the ComputeShader).
		pDeviceContext->CSSetShader(m_VerticalBlur->GetComputeShader(), NULL, 0);
		UINT numGroupsY = (UINT)ceilf(m_Height / 256.0f);
		pDeviceContext->Dispatch(m_Width, numGroupsY, 1);

		pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	}

	// Disable compute shader.
	pDeviceContext->CSSetShader(0, 0, 0);
}

void BlurFilter::Apply(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputSRV, 
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> inputUAV, 
	UINT denominator,
	unsigned int count)
{
	UINT _nowWidth = m_Width / denominator;
	UINT _nowHeight = m_Height / denominator;

	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	for (unsigned int i = 0; i < count; ++i)
	{
		// 쉐이더 자원 묶기
		// Input
		ID3D11ShaderResourceView* arrViews[1] = { inputSRV.Get() };
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);

		// Output
		ID3D11UnorderedAccessView* arrUAVs[1] = { m_pBlurredOutputUAV.Get() };
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		// HORIZONTAL blur pass.
		pDeviceContext->CSSetShader(m_HorizontalBlur->GetComputeShader(), NULL, 0);

		// How many groups do we need to dispatch to cover a row of pixels, where each
		// group covers 256 pixels (the 256 is defined in the ComputeShader).
		// m_Width 화면 사이즈가 아니라 마지막으로 샘플링한 렌더 타겟의 사이즈이다
		UINT numGroupsX = (UINT)ceilf(_nowWidth / 256.0f);
		pDeviceContext->Dispatch(numGroupsX, _nowHeight, 1);

		pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);


		// 쉐이더 자원 묶기
		arrViews[0] = { m_pBlurredOutputSRV.Get() };
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);

		arrUAVs[0] = { inputUAV.Get() };
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		// VERTICAL blur pass.
		// How many groups do we need to dispatch to cover a column of pixels, where each
		// group covers 256 pixels  (the 256 is defined in the ComputeShader).
		pDeviceContext->CSSetShader(m_VerticalBlur->GetComputeShader(), NULL, 0);
		UINT numGroupsY = (UINT)ceilf(_nowHeight / 256.0f);
		pDeviceContext->Dispatch(_nowWidth, numGroupsY, 1);

		pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	}

	// Disable compute shader.
	pDeviceContext->CSSetShader(0, 0, 0);
}

void BlurFilter::Apply(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputSRV, 
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> inputUAV, 
	UAVRenderTargetView* pBuffer, UINT denominator, unsigned int count)
{
	UINT _nowWidth = m_Width / denominator;
	UINT _nowHeight = m_Height / denominator;

	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	for (unsigned int i = 0; i < count; ++i)
	{
		// 쉐이더 자원 묶기
		// Input
		ID3D11ShaderResourceView* arrViews[1] = { inputSRV.Get() };
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);

		// Output
		ID3D11UnorderedAccessView* arrUAVs[1] = { pBuffer->GetUnorderedAccessViewRawptr() };
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		// HORIZONTAL blur pass.
		pDeviceContext->CSSetShader(m_HorizontalBlur->GetComputeShader(), NULL, 0);

		// How many groups do we need to dispatch to cover a row of pixels, where each
		// group covers 256 pixels (the 256 is defined in the ComputeShader).
		// m_Width 화면 사이즈가 아니라 마지막으로 샘플링한 렌더 타겟의 사이즈이다
		UINT numGroupsX = (UINT)ceilf(_nowWidth / 256.0f); 
		pDeviceContext->Dispatch(numGroupsX, _nowHeight, 1);

		pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);


		// 쉐이더 자원 묶기
		arrViews[0] = { pBuffer->GetShaderResourceViewRawptr() };
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);

		arrUAVs[0] = { inputUAV.Get() };
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		// VERTICAL blur pass.
		// How many groups do we need to dispatch to cover a column of pixels, where each
		// group covers 256 pixels  (the 256 is defined in the ComputeShader).
		pDeviceContext->CSSetShader(m_VerticalBlur->GetComputeShader(), NULL, 0);
		UINT numGroupsY = (UINT)ceilf(_nowHeight / 256.0f);
		pDeviceContext->Dispatch(_nowWidth, numGroupsY, 1);

		pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	}

	// Disable compute shader.
	pDeviceContext->CSSetShader(0, 0, 0);
}

void BlurFilter::ApplyFactor(const IRenderOption& option)
{
	if (m_BlurCount != option.blurCount)
	{
		m_BlurCount = option.blurCount;
	}


}

void BlurFilter::SetGaussianWeghts(float sigma)
{
	float d = 2.0f * sigma * sigma;

	float weights[9];
	float sum = 0.0f;
	for (int i = 0; i < 8; ++i)
	{
		float x = (float)i;
		weights[i] = expf(-x * x / d);

		sum += weights[i];
	}

	// Divide by the sum so all the weights add up to 1.0.
	for (int i = 0; i < 8; ++i)
	{
		weights[i] /= sum;
	}

	for (int i = 0; i < 8; ++i)
	{
		//ConstantBufferManager::GetCSBlurBuffer()->data.weight[i] = weights[i];
	}
	//ConstantBufferManager::GetCSBlurBuffer()->data.radius = 5.0f;
	ConstantBufferManager::GetCSBloomBuffer()->ApplyChanges();
	

}
