#pragma once

#include "D3D11Define.h"
#include "PostProcessBase.h"

class BungieBloom : public PostProcessBase
{
public:
	BungieBloom();
	virtual ~BungieBloom();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);
	void OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);

	void ApplyBoomCurve(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
		UAVRenderTargetView* renderTarget);

	void ColorCalb(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		UAVRenderTargetView* pSource, UAVRenderTargetView* pTarget);

	void UpScaleStack(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		UAVRenderTargetView* lower, UAVRenderTargetView* upper);

public:
	virtual void ApplyFactor(const IRenderOption& option) override;


private:
	void CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	void CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	UINT m_Width;
	UINT m_Height;

	unsigned int m_BlurCount;

	float m_Threshold;
	float m_Intensity;
	float m_AdjustFactor;

	std::shared_ptr<ComputeShader> m_BloomCurve;
	std::shared_ptr<ComputeShader> m_UpScalShader;
	std::shared_ptr<ComputeShader> m_HorizontalBlur;
	std::shared_ptr<ComputeShader> m_VerticalBlur;

	std::shared_ptr<ComputeShader> m_ColorCalb;
	std::shared_ptr<ComputeShader> m_Copy;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pBloomCurvedUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pBloomCurvedSRV;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pTempUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pTempSRV;

};

