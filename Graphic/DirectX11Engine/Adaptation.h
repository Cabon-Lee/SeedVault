#pragma once
#include "HighDynamicRangeRender.h"


class Adaptation : public HighDynamicRangeRender
{
public:
	Adaptation();
	virtual ~Adaptation();

	virtual void Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height) override;


	// Present«“ ∑ª¥ı ≈∏∞Ÿ∞˙ Ω∫ø“ √º¿Œ¿« ∑ª¥ı ≈∏∞Ÿ¿ª ≥—∞‹¡ÿ¥Ÿ
	virtual void Apply(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::shared_ptr<RenderTargetView> pRenderTarget,
		std::shared_ptr<RenderTargetView> pSwapChainRenderTarget) override;

	virtual void Apply(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::shared_ptr<RenderTargetView> pRenderTarget,
		RenderTargetView* pSwapChainRenderTarget) override;

	virtual void OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height) override;

public:
	virtual void ApplyFactor(const IRenderOption& option) override;

protected:
	virtual void CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice) override;
	virtual void DownScale(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResource) override;

	virtual void FinalPass(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResource) override;

private:
	void CreateExtraBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);


private:
	float m_Adaptation;

private:
	// Previous average luminance for adaptation
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pPrevAverageLuminanceBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pPrevAverageLuminanceUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pPrevAverageLuminanceSRV;

};

