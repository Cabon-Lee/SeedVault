#include "BloomFilter.h"

#include "Shaders.h"
#include "UAVRenderTargetView.h"

#include "PostProcessCB.h"

#include "RasterizerState.h"

BloomFilter::BloomFilter()
{

}

BloomFilter::~BloomFilter()
{

}

void BloomFilter::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width; m_Height = height;

	CreateBuffer(pDevice);
	CreateShader(pDevice);
}

void BloomFilter::OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height)
{
	m_Width = width; m_Height = height;

	CreateBuffer(pDevice);
}

void BloomFilter::Filtering(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	ID3D11ShaderResourceView* pSRV)
{
	ID3D11ShaderResourceView* arrViews[1] = { pSRV };
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pBloomFilterUAV.Get() };

	UINT numGroupsX = (UINT)ceilf(static_cast<float>(m_Width) / 32);
	UINT numGroupsY = (UINT)ceilf(static_cast<float>(m_Height) / 32);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pDeviceContext->Map(m_pFilterCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	CB_CS_BloomFilter* pDownScale = (CB_CS_BloomFilter*)MappedResource.pData;
	pDownScale->fHuddle = m_Huddle;
	pDownScale->fIncrease = m_Increase;

	pDeviceContext->Unmap(m_pFilterCB.Get(), 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pFilterCB.Get() };
	pDeviceContext->CSSetConstantBuffers(0, 1, arrConstBuffers);


	// 병합을 실행해서 이 클래스가 가지고 있는 UAV에 텍스처를 작성한다
	{
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_BloomFilter->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);

	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	pDeviceContext->PSSetConstantBuffers(0, 1, arrConstBuffers);
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	pDeviceContext->CSSetShader(0, 0, 0);
}

void BloomFilter::Filtering(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	ID3D11ShaderResourceView* pSRV, 
	UAVRenderTargetView* pUAVRTV)
{
	ID3D11ShaderResourceView* arrViews[1] = { pSRV };
	ID3D11UnorderedAccessView* arrUAVs[1] = { pUAVRTV->GetUnorderedAccessViewRawptr() };

	UINT numGroupsX = (UINT)ceilf(pUAVRTV->GetViewPort().Width / 32);
	UINT numGroupsY = (UINT)ceilf(pUAVRTV->GetViewPort().Height / 32);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pDeviceContext->Map(m_pFilterCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	CB_CS_BloomFilter* pDownScale = (CB_CS_BloomFilter*)MappedResource.pData;
	pDownScale->fHuddle = m_Huddle;
	pDownScale->fIncrease = m_Increase;

	pDeviceContext->Unmap(m_pFilterCB.Get(), 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pFilterCB.Get() };
	pDeviceContext->CSSetConstantBuffers(0, 1, arrConstBuffers);


	// 병합을 실행해서 이 클래스가 가지고 있는 UAV에 텍스처를 작성한다
	{
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		pDeviceContext->CSSetShader(m_BloomFilter->GetComputeShader(), NULL, 0);
		pDeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}

	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	pDeviceContext->CSSetShaderResources(0, 1, nullSRV);

	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	pDeviceContext->PSSetConstantBuffers(0, 1, arrConstBuffers);
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	pDeviceContext->CSSetShader(0, 0, 0);
}

void BloomFilter::Blur(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	ID3D11ShaderResourceView* arrViews[1];
	ID3D11UnorderedAccessView* arrUAVs[1];

	UINT numGroupsX = (UINT)ceilf(static_cast<float>(m_Width) / 32);
	UINT numGroupsY = (UINT)ceilf(static_cast<float>(m_Height) / 32);

	for (unsigned int i = 0; i < m_BlurCount; ++i)
	{
		// 쉐이더 자원 묶기
		// Input
		arrViews[0] = { m_pBloomFilterSRV.Get() };
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);

		// Output
		arrUAVs[0] = { m_pBlurUAV.Get() };
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		// HORIZONTAL blur pass.
		pDeviceContext->CSSetShader(m_HorizontalBlur->GetComputeShader(), NULL, 0);

		// How many groups do we need to dispatch to cover a row of pixels, where each
		// group covers 256 pixels (the 256 is defined in the ComputeShader).
		// m_Width 화면 사이즈가 아니라 마지막으로 샘플링한 렌더 타겟의 사이즈이다
		numGroupsX = (UINT)ceilf(m_Width / 256.0f);
		pDeviceContext->Dispatch(numGroupsX, m_Height, 1);

		// Unbind the input texture from the CS for good housekeeping.
		ID3D11ShaderResourceView* nullSRV[1] = { 0 };
		pDeviceContext->CSSetShaderResources(0, 1, nullSRV);

		// Unbind output from compute shader (we are going to use this output as an input in the next pass, 
		// and a resource cannot be both an output and input at the same time.
		ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);


		// 쉐이더 자원 묶기
		arrViews[0] = { m_pBlurSRV.Get() };
		pDeviceContext->CSSetShaderResources(0, 1, arrViews);

		arrUAVs[0] = { m_pBloomFilterUAV.Get() };
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, (UINT*)(&arrUAVs));

		// VERTICAL blur pass.
		// How many groups do we need to dispatch to cover a column of pixels, where each
		// group covers 256 pixels  (the 256 is defined in the ComputeShader).
		pDeviceContext->CSSetShader(m_VerticalBlur->GetComputeShader(), NULL, 0);
		numGroupsY = (UINT)ceilf(m_Height / 256.0f);
		pDeviceContext->Dispatch(m_Width, numGroupsY, 1);

		pDeviceContext->CSSetShaderResources(0, 1, nullSRV);
		pDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	}

	pDeviceContext->CSSetShader(0, 0, 0);
}

void BloomFilter::Merge(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	ID3D11ShaderResourceView* pSource)
{
	ID3D11ShaderResourceView* arrViews[2] = { pSource, m_pBloomFilterSRV.Get() };
	pDeviceContext->PSSetShaderResources(0, 2, arrViews);

	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11SamplerState* arrSamplers[2] =
	{
		RasterizerState::GetPointSamplerState(), RasterizerState::GetLinearSamplerState()
	};
	pDeviceContext->PSSetSamplers(0, 2, arrSamplers);

	// Set the shaders
	pDeviceContext->VSSetShader(m_pFullScreenQuadVS->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(m_Merge->GetPixelShader(), NULL, 0);

	pDeviceContext->Draw(4, 0);

	// Cleanup
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->PSSetShaderResources(0, 2, arrViews);
	pDeviceContext->VSSetShader(NULL, NULL, 0);
	pDeviceContext->PSSetShader(NULL, NULL, 0);
}

void BloomFilter::Merge(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, ID3D11ShaderResourceView* pSource, ID3D11ShaderResourceView* pSource2)
{
	ID3D11ShaderResourceView* arrViews[2] = { pSource, pSource2 };
	pDeviceContext->PSSetShaderResources(0, 2, arrViews);

	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11SamplerState* arrSamplers[2] =
	{
		RasterizerState::GetLinearSamplerState(), RasterizerState::GetPointSamplerState(), 
	};
	pDeviceContext->PSSetSamplers(0, 2, arrSamplers);

	// Set the shaders
	pDeviceContext->VSSetShader(m_pFullScreenQuadVS->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(m_Merge->GetPixelShader(), NULL, 0);

	pDeviceContext->Draw(4, 0);

	// Cleanup
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->PSSetShaderResources(0, 2, arrViews);
	pDeviceContext->VSSetShader(NULL, NULL, 0);
	pDeviceContext->PSSetShader(NULL, NULL, 0);
}

void BloomFilter::ApplyFactor(const IRenderOption& option)
{
	m_BlurCount = option.blurCount;
	m_Increase = option.intensity;
}

void BloomFilter::CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_pBloomFilterUAV.Reset();
	m_pBloomFilterSRV.Reset();
	m_pBlurUAV.Reset();
	m_pBlurSRV.Reset();

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
		_hr = pDevice->CreateShaderResourceView(_blurredTex.Get(), &srvDesc,
			m_pBlurSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		_hr = pDevice->CreateUnorderedAccessView(_blurredTex.Get(), &uavDesc,
			m_pBlurUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView Fail");

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	 _bloomTex;
		_hr = pDevice->CreateTexture2D(&_blurredTexDesc, 0, _bloomTex.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateTexture2D BloomTex Fail");

		_hr = pDevice->CreateShaderResourceView(_bloomTex.Get(), &srvDesc,
			m_pBloomFilterSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView  m_pBlurSRV Fail");

		_hr = pDevice->CreateUnorderedAccessView(_bloomTex.Get(), &uavDesc,
			m_pBloomFilterUAV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "CreateUnorderedAccessView  m_pBlurUAV Fail");

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.ByteWidth = sizeof(CB_CS_BloomFilter);
		_hr = pDevice->CreateBuffer(&bufferDesc, NULL, m_pFilterCB.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateBuffer  m_pFilterCB Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}

void BloomFilter::CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_BloomFilter = std::make_shared<ComputeShader>();
	m_BloomFilter->Initialize(pDevice, L"../Data/Shader/cs_BloomFilter.hlsl", "BloomFilter", "cs_5_0");

	m_HorizontalBlur = std::make_shared<ComputeShader>();
	m_HorizontalBlur->Initialize(pDevice, L"../Data/Shader/cs_gaussianBlur.hlsl", "HorzBlurCS", "cs_5_0");

	m_VerticalBlur = std::make_shared<ComputeShader>();
	m_VerticalBlur->Initialize(pDevice, L"../Data/Shader/cs_gaussianBlur.hlsl", "VertBlurCS", "cs_5_0");

	m_Merge = std::make_shared<PixelShader>();
	m_Merge->Initialize(pDevice, L"../Data/Shader/ps_PostProcess.hlsl", "Merge", "ps_5_0");

}
