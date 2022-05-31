#pragma once
#include "RenderTargetBase.h"

class RenderTargetView;

class CubeMapRenderTarget : public RenderTargetBase
{
public:
	CubeMapRenderTarget();
	virtual ~CubeMapRenderTarget();

	void Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice, unsigned int width, unsigned int height, unsigned int mipMaps);
	
	Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetTexture2D();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceViewRawptr();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRenderTargetView(unsigned int idx);
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDepthStencilView();

	void CreateMipMap(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
		std::vector<std::shared_ptr<RenderTargetView>>& pRenderTargets,	// 독립적인 렌더타겟들을 하나씩 받는다
		unsigned int mipWidth, unsigned int mipHeight, unsigned int mipSlice);

	void CreateViewPort(UINT width, UINT height);

private:
	Microsoft::WRL::ComPtr <ID3D11RenderTargetView> m_pRenderTargetView[6];
	Microsoft::WRL::ComPtr <ID3D11DepthStencilView> m_pDepthStencilView;

	unsigned int m_MipMaps;
};

