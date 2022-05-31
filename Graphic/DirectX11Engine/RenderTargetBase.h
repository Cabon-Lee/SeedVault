#pragma once

/// ���� Ÿ�� ���� ������ ���� �ʱ�ȭ�ϱ� ����, �� ����ϱ� ���ϰ� ���ε� Ŭ����
/// ������ ������ ����Ÿ�� �йи��� �ڵ� �ߺ��� ����, �������� abstact�� ������ ��찡 �־���.
/// ���⼭�� ���������� ���Ǵ� �ʱ�ȭ �Լ��� �ϳ��� ���� ���װ�, ���������� ������ �ִ� ��� ������ Base���� ��ȯ�ϵ��� �ߴ�.

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

	// �������� ���Ǵ� �������
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pRTVTexture2D;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSharderResourceView;
	D3D11_VIEWPORT m_ViewPort;
};

