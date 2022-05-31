#pragma once

#include "PostProcessBase.h"

class FXAA : public PostProcessBase
{
public:
	FXAA();
	virtual ~FXAA();
	virtual void ApplyFactor(const IRenderOption& option) override;

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	void Apply(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> pSRV,
		UINT width, UINT height);

private:
	std::unique_ptr<class PixelShader> m_FXAAPixelShader;

};

