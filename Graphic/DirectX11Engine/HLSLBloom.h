#pragma once

#include "PostProcessBase.h"

class HLSLBloom : public PostProcessBase
{
public:
	HLSLBloom();
	virtual ~HLSLBloom();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);
	void OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);

	void BindConstantBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	void ClearBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);

	// 다운 샘플링을 해준다
	void DownScale(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV);

	// Extract the bloom values from the downscaled image
	void Bloom(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);

	// Apply a gaussian blur to the input and store it in the output
	void Blur(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pInput, 
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pOutput);

	void Blur(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);

	// Final pass that composites all the post processing calculations
	void FinalPass(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pHDRSRV);

	void BufferSwap();

public:
	virtual void ApplyFactor(const IRenderOption& option) override;


private:
	void CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	void CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	void CreateRecycleBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	UINT m_Width;
	UINT m_Height;
	UINT m_DownScaleGroups;

	float m_MiddleGrey;
	float m_White;
	float m_BloomScale;
	float m_Adaption;
	float m_BloomThreshold;

	std::shared_ptr<ComputeShader> m_DownFirstScale;
	std::shared_ptr<ComputeShader> m_DownSecondScale;
	std::shared_ptr<ComputeShader> m_BloomShader;
	std::shared_ptr<ComputeShader> m_VerticalBlur;
	std::shared_ptr<ComputeShader> m_HorizontalBlur;

	// Downscaled scene texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDownScaleRT;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pDownScaleSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pDownScaleUAV;

	// Temporary texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTempRT[2];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTempSRV[2];
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pTempUAV[2];

	// Bloom texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pBloomRT;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pBloomSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pBloomUAV;

	// 1D intermediate storage for the down scale operation
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pDownScale1DBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pDownScale1DUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pDownScale1DSRV;

	// Average luminance
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pAvgLumBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pAvgLumUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pAvgLumSRV;

	// Previous average luminance for adaptation
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pPrevAvgLumBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pPrevAvgLumUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pPrevAvgLumSRV;

	Microsoft::WRL::ComPtr < ID3D11Buffer> m_pDownScaleCB;
	Microsoft::WRL::ComPtr < ID3D11Buffer> m_pFinalPassCB;
	Microsoft::WRL::ComPtr < ID3D11Buffer> m_pBlurCB;

};

