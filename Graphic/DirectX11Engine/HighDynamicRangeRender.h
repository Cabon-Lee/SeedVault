#pragma once

#include <memory>
#include "D3D11Define.h"
#include "PostProcessBase.h"

class VertexShader;
class PixelShader;
class ComputeShader;
class RenderTargetView;
class UAVRenderTargetView;



class HighDynamicRangeRender : public PostProcessBase
{
public:
	HighDynamicRangeRender();
	~HighDynamicRangeRender();

	virtual void Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);

	// Present할 렌더 타겟과 스왑 체인의 렌더 타겟을 넘겨준다
	virtual void Apply(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::shared_ptr<RenderTargetView> pRenderTarget,
		std::shared_ptr<RenderTargetView> pSwapChainRenderTarget);

	virtual void Apply(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::shared_ptr<RenderTargetView> pRenderTarget,
		RenderTargetView* pSwapChainRenderTarget);

	virtual void OnResize(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);

public:
	virtual void ApplyFactor(const IRenderOption& option) override;

protected:
	void CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	virtual void CreateShader(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	virtual void DownScale(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResource);
	virtual void FinalPass(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResource);

	

protected:
	UINT m_Width;
	UINT m_Height;

	UINT m_DownScaleGroups;

	float m_MiddleGrey;
	float m_White;

	// 일단은 사용할 쉐이더를 직접 써넣겠다
	std::shared_ptr<ComputeShader> m_HDRFirstPassShader;
	std::shared_ptr<ComputeShader> m_HDRSecondPassShader;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_pDownScale1DBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pDownScale1DUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pDownScale1DSRV;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_pAverageLuminanceBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	m_pAverageLuminanceDUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pAverageLuminanceDSRV;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_pDownScaleCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_pFinalPassCB;

};

