#pragma once
#include "PostProcessBase.h"
class Sampler :
    public PostProcessBase
{
public:
    Sampler();
    virtual ~Sampler();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);
	void OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);

	void DownSample(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pSource);

	void DownSampleEX(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pSource);

	void UpSample(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pSource, ID3D11UnorderedAccessView* pTarget,
		UINT denominator);

	void UpSample(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pSource);

	void BilinearSample(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pSource, ID3D11UnorderedAccessView* pTarget);

	void Merge(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pBlured, ID3D11ShaderResourceView* pOrigin,
		ID3D11UnorderedAccessView* pTarget,
		UINT denominator);

	void Merge(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pBlured, ID3D11ShaderResourceView* pOrigin,
		ID3D11UnorderedAccessView* pTarget);

	void PixelMerge(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pBlured, ID3D11ShaderResourceView* pOrigin,
		ID3D11RenderTargetView* pRenderTarget);


	void UpSampleAndMerge(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pUpsampleSource, 
		ID3D11ShaderResourceView* pMergeSource,
		ID3D11UnorderedAccessView* pTarget,
		UINT denominator);


	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pFirst4x4UAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pFirst4x4SRV;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pFirst6x6UAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pFirst6x6SRV;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pSecond6x6UAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSecond6x6SRV;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pUpFirst4x4UAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pUpFirst4x4SRV;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pUpFirst6x6UAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pUpFirst6x6SRV;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pUpSecond6x6UAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pUpSecond6x6SRV;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pMergeUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pMergeSRV;

public:
	virtual void ApplyFactor(const IRenderOption& option) override;

private:
	void CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	void CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	UINT m_Width;
	UINT m_Height;

	std::shared_ptr<ComputeShader> m_DownSample4x4;
	std::shared_ptr<ComputeShader> m_DownSample6x6;

	std::shared_ptr<ComputeShader> m_UpSample4x4;
	std::shared_ptr<ComputeShader> m_UpSample6x6;
	std::shared_ptr<ComputeShader> m_UpSampleBilinear;

	std::shared_ptr<ComputeShader> m_Merge;
	std::shared_ptr<ComputeShader> m_Copy;

	std::shared_ptr<PixelShader> m_PixelMerge;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pTempUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pTempSRV;



};

