#pragma once
#include "RenderTargetView.h"

class UAVRenderTargetView :
    public RenderTargetView
{

public:
    UAVRenderTargetView();
    virtual ~UAVRenderTargetView();

	void Initialize(
		Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
		UINT width,
		UINT height);

	ID3D11UnorderedAccessView* GetUnorderedAccessViewRawptr();
	ID3D11UnorderedAccessView* const* GetUnorderedAccessViewAddressOf();
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> GetUnorderedAccessView();

private:
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pUnorderedAccessView;

};

