#pragma once

/// 렌더 타겟 류를 종류에 따라 초기화하기 쉽게, 또 사용하기 편하게 감싸둔 클래스
/// 이전에 만들어둔 렌더타겟 패밀리가 코드 중복도 많고, 쓸데없이 abstact로 만들어둔 경우가 있었다.
/// 여기서는 공통적으로 사용되는 초기화 함수를 하나로 통일 시켰고, 공통적으로 가지고 있는 멤버 변수를 Base에서 반환하도록 했다.

#include "D3D11Define.h"
#include "ErrorLogger.h"
#include "COMException.h"

class RenderTargetBase
{
public:
	RenderTargetBase();
	virtual ~RenderTargetBase();

public:
	ID3D11ShaderResourceView* GetShaderResourceViewRawptr();
	ID3D11ShaderResourceView* const* GetShaderResourceViewAddressOf();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView();

	Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetTexture2D();

	const D3D11_VIEWPORT& GetViewPort();


protected:
	HRESULT CreateResource(
		Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
		UINT width, 
		UINT height,
		UINT bindFlags,
		DXGI_FORMAT textureFormat,
		DXGI_FORMAT shaderResourceFormat,
		D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
		UINT cpuFlag = 0);

	void BaseReset();

private:
	HRESULT CreateTexture2D(ID3D11Device* pDevice, DXGI_FORMAT textureFormat, UINT bindFlags);
	HRESULT CreateTexture2D(ID3D11Device* pDevice, DXGI_FORMAT textureFormat, UINT bindFlags, 
		D3D11_USAGE usage, UINT cpuFlag);
	void CreateViewPort();

protected:
	UINT m_Width;
	UINT m_Height;

	// 공통으로 사용되는 멤버변수
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pRTVTexture2D;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSharderResourceView;
	D3D11_VIEWPORT m_ViewPort;
};

