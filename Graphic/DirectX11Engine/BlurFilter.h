#pragma once

#include "D3D11Define.h"
#include "PostProcessBase.h"

class BlurFilter : public PostProcessBase
{
public:
	BlurFilter();
	~BlurFilter();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);
	void OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);

	void Apply(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputSRV,
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> inputUAV);

	void Apply(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputSRV,
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> inputUAV,
		UINT denominator,
		unsigned int count);

	void Apply(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> inputSRV,
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> inputUAV,
		UAVRenderTargetView* pBuffer,
		UINT denominator,
		unsigned int count);


public: 
	virtual void ApplyFactor(const IRenderOption& option) override;

private:
	void CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	void SetGaussianWeghts(float sigma);

	UINT m_Width;
	UINT m_Height;
	int m_BlurCount;
	unsigned int m_GaussianWeight;

	// 일단은 사용할 쉐이더를 직접 써넣겠다
	std::shared_ptr<ComputeShader> m_HorizontalBlur;
	std::shared_ptr<ComputeShader> m_VerticalBlur;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pBlurredOutputUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pBlurredOutputSRV;


	Microsoft::WRL::ComPtr <ID3D11Buffer> m_CS_Buffer;

};

