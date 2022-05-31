#pragma once

#include "D3D11Define.h"
#include "ErrorLogger.h"
#include "COMException.h"

class RenderTargetView;
class UAVRenderTargetView;
class VertexShader;
class PixelShader;

const unsigned int FULL_SAMPLE_SCALE = 1024;
const unsigned int SAMPLE_SCALE = 6;
const UINT SCREEN_SIZE[6] = { 1024, 512, 256, 128, 64, 32 };


class DownSampler
{
public:
	DownSampler();
	~DownSampler();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader);
	void BindRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, const unsigned int RTVindex);

	void DownSample(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, RenderTargetView* pTarget);
	void UpSample(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, RenderTargetView* pSwapChain);

	void UpScaleCopy(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		unsigned int scale,
		RenderTargetView* pSource);

	void DownScaleCopy(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		unsigned int scale,
		RenderTargetView* pSource);

	void DownAndUpSamepling(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, RenderTargetView* pTarget, RenderTargetView* pDrawing);

	void Sampling(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		RenderTargetView* pRTVToSample);
	
	RenderTargetView* GetTempSourceRawptr();
	RenderTargetView* GetSampleRawptr(const unsigned int RTVindex);

	std::shared_ptr<UAVRenderTargetView> GetSample(const unsigned int RTVindex);

public:
	UINT m_SampleScale;

private:
	int m_TempSampleCount;
	RenderTargetView* m_pTempSource;

	// 다운 샘플링 크기가 바뀌면 여기서 다시 만들기 위해
	std::vector<std::shared_ptr<UAVRenderTargetView>>	m_SamplingRTV_V;
	std::shared_ptr<VertexShader>			m_VertexShader;
	std::shared_ptr<PixelShader>			m_PixelShader;
};

