#pragma once
#include "PostProcessBase.h"
class UpScale : public PostProcessBase
{
public:
	UpScale();
	virtual ~UpScale();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);
	void OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);

	void Apply(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		UAVRenderTargetView* inputRTV,
		UAVRenderTargetView* outputRTV);

public:
	virtual void ApplyFactor(const IRenderOption& option) override;


private:
	void CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	void CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	UINT m_Width;
	UINT m_Height;

	std::shared_ptr<ComputeShader> m_UpScalShader;
	std::shared_ptr<ComputeShader> m_Copy;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pUpScaleUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pUpScaleSRV;
};

