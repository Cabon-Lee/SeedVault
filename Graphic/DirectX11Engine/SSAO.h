#pragma once

#include "D3D11Define.h"

class SSAO
{
public:
	SSAO();
	~SSAO();

	void Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::shared_ptr<class PixelShader> pSSAOPS,
		std::shared_ptr<class VertexShader> pSSAOVS,
		std::shared_ptr<class PixelShader> pSSAOBlurPS,
		std::shared_ptr<class VertexShader> pSSAOBlurVS,
		UINT width, UINT height, float fovy, float farZ);

	///<summary>
	/// Call when the backbuffer is resized.  
	///</summary>
	void OnSize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height, float fovy, float farZ);
	void ReCalcCamera(UINT width, UINT height, float fovy, float farZ);

	///<summary>
	/// Changes the render target to the NormalDepth render target.  Pass the 
	/// main depth buffer as the depth buffer to use when we render to the
	/// NormalDepth map.  This pass lays down the scene depth so that there in
	/// no overdraw in the subsequent rendering pass.
	///</summary>
	void SetNormalDepthRenderTarget(ID3D11DepthStencilView* dsv);

	///<summary>
	/// Changes the render target to the Ambient render target and draws a fullscreen
	/// quad to kick off the pixel shader to compute the AmbientMap.  We still keep the
	/// main depth buffer binded to the pipeline, but depth buffer read/writes
	/// are disabled, as we do not need the depth buffer computing the Ambient map.
	///</summary>
	void ComputeSsao(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		const DirectX::XMMATRIX& proj,
		ID3D11ShaderResourceView* pSRV);

	///<summary>
	/// Blurs the ambient map to smooth out the noise caused by only taking a
	/// few random samples per pixel.  We use an edge preserving blur so that 
	/// we do not blur across discontinuities--we want edges to remain edges.
	///</summary>
	void BlurAmbientMap(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		int blurCount);

	Microsoft::WRL::ComPtr<ID3D11Buffer>& GetScreenQuadVB();
	Microsoft::WRL::ComPtr<ID3D11Buffer>& GetScreenQuadIB();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetRandomVectorSRV();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& AmbientSRV();

private:
	void BlurAmbientMap(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* inputSRV, ID3D11RenderTargetView* outputRTV, bool horzBlur);

	void BuildFrustumFarCorners(float fovy, float farZ);

	void BuildFullScreenQuad(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	void BuildTextureViews(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	void ReleaseTextureViews();

	void BuildRandomVectorTexture(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	void BuildOffsetVectors();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_ScreenQuadVB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_ScreenQuadIB;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_RandomVectorSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_NormalDepthRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_NormalDepthSRV;

	// Need two for ping-ponging during blur.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_AmbientRTV0;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_AmbientSRV0;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_AmbientRTV1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_AmbientSRV1;


	UINT m_RenderTargetWidth;
	UINT m_RenderTargetHeight;

	std::shared_ptr<class PixelShader>  m_pComputeSSAOPS;
	std::shared_ptr<class VertexShader> m_pComputeSSAOVS;
	std::shared_ptr<class PixelShader>  m_pBlurSSAOPS;
	std::shared_ptr<class VertexShader> m_pBlurSSAOVS;


	DirectX::XMFLOAT4 m_FrustumFarCorner[4];
	float m_OffsetsFloat[56];

	D3D11_VIEWPORT m_AmbientMapViewport;

	ID3D11ShaderResourceView* m_pTempDepthMap;
};

