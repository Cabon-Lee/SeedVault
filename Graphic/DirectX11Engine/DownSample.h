#pragma once

#include "D3D11Define.h"
#include "ErrorLogger.h"
#include "COMException.h"

class RenderTargetView;
class VertexShader;
class PixelShader;

const unsigned int FULL_SAMPLE_SCALE = 1024;

class DownSample
{
public:
	DownSample();
	~DownSample();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);
	void BindRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	void Sampling(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::shared_ptr<RenderTargetView> pRTVToSample);

	std::shared_ptr<RenderTargetView> GetDownedRenderTarget();


protected:
	UINT m_SampleScale;

	// �ٿ� ���ø� ũ�Ⱑ �ٲ�� ���⼭ �ٽ� ����� ����
	Microsoft::WRL::ComPtr<ID3D11Device>	m_pDevice;
	std::shared_ptr<RenderTargetView>		m_SamplingRTV;
	std::shared_ptr<VertexShader>			m_VertexShader;
	std::shared_ptr<PixelShader>			m_PixelShader;
};

