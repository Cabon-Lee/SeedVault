#pragma once

#include "D3D11Define.h"

class RenderTargetView;
class UAVRenderTargetView;
class VertexShader;
class PixelShader;

const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

class OffScreenRenderTarget
{
public:
	OffScreenRenderTarget();
	~OffScreenRenderTarget();

	void Initialize(Microsoft::WRL::ComPtr <ID3D11Device> pDevice, UINT width, UINT height,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader);
	void OnResize(Microsoft::WRL::ComPtr <ID3D11Device> pDevice, UINT width, UINT height);

	void Copy(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		ID3D11ShaderResourceView* pSource);
	void Copy(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::shared_ptr<RenderTargetView> pRenderTarget);
	void CopyTo(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, std::shared_ptr<RenderTargetView> pRenderTarget);
	void Copy(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		RenderTargetView* pRenderTarget);
	void CopyTo(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, RenderTargetView* pRenderTarget);
	void CopyFromTo(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		RenderTargetView* pSource, RenderTargetView* pTarget);
	void CopyFromTo(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSource, RenderTargetView* pTarget);

	std::shared_ptr<UAVRenderTargetView> GetUAVRenderTarget();

private:
	void RenderTargetBind(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
		const D3D11_VIEWPORT& viewPort,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV);

	// 복사 그리기를 실행한다
	void CopyProcess(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);

	UINT m_Width, m_Height;

	std::shared_ptr<VertexShader> m_VertexShader;
	std::shared_ptr<PixelShader> m_PixelShader;

	std::shared_ptr<UAVRenderTargetView> m_pUAVRenderTarget;

};

