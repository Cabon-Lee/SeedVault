#pragma once

#include "PostProcessBase.h"

class BloomFilter : public PostProcessBase
{
public:
	BloomFilter();
	virtual ~BloomFilter();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);
	void OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);

	void Filtering(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pSRV);

	void Filtering(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pSRV,
		UAVRenderTargetView* pUAVRTV);

	void Blur(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	void Merge(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pSource);
	void Merge(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pSource,
		ID3D11ShaderResourceView* pSource2);

public:
	void ApplyFactor(const IRenderOption& option) override;

private:
	void CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	void CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	UINT m_Width;
	UINT m_Height;

	unsigned int m_BlurCount;

	float m_Huddle;
	float m_Increase;

	std::shared_ptr<ComputeShader> m_BloomFilter;
	std::shared_ptr<ComputeShader> m_HorizontalBlur;
	std::shared_ptr<ComputeShader> m_VerticalBlur;

	std::shared_ptr<PixelShader> m_Merge;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pBloomFilterUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pBloomFilterSRV;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pBlurUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pBlurSRV;

	Microsoft::WRL::ComPtr <ID3D11Buffer> m_pFilterCB;

};

