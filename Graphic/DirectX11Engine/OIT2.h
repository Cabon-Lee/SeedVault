#pragma once
#include <memory>
#include <vector>

#include "ConstantBuffer.h"

__interface IResourceManager;
class VertexShader;
class PixelShader;
class Object;

class OIT2
{
public:
	OIT2();
	//~OIT2();

	/*
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, 
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
		std::shared_ptr<IResourceManager> pResourceManager);
	void Run(const DirectX::XMMATRIX& worldTM,
		const DirectX::XMMATRIX& viewTM,
		const DirectX::XMMATRIX& projTM, 
		const DirectX::XMFLOAT3 camPos, 
		const std::vector<Object*>& objs, int nPic);
	void Render(int iIndex = -1);
	void GetSRV(int index, ID3D11ShaderResourceView** ppSRV);
	void GetDSV(int index, ID3D11DepthStencilView** ppDSV);
	void GetRTV(int index, ID3D11RenderTargetView** ppRTV);
private:
	struct PictureInfo
	{
		ID3D11DepthStencilView* dsv;
		ID3D11ShaderResourceView* depthSrv;
		ID3D11RenderTargetView* rtv;
		ID3D11ShaderResourceView* backSrv;
	};
	void Create(PictureInfo* picInfo);
	void PreRun(int count);
	*/

	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	std::shared_ptr<IResourceManager> m_pResourceManager;

	ID3D11BlendState* m_dxBlendState;
	ID3D11RasterizerState* m_dxRState;
	ID3D11DepthStencilState* m_dxDSState;

	ID3D11Texture2D* m_backBuffer;
	ID3D11Texture2D* m_depthStencilBuffer;
	
	ID3D11RenderTargetView* m_rtv;
	ID3D11DepthStencilView* m_dsView;

	std::shared_ptr<VertexShader> m_vs;
	std::shared_ptr<PixelShader> m_ps;

	ID3D11ShaderResourceView* m_nullSRV;
	//std::vector<PictureInfo> m_pic_V;

	ConstantBuffer<CB_PS_OIT_CAMPOS>* m_cbEye;

	Object* m_renderQuad;


};

