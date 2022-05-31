#pragma once

/// 2022.03.24
/// �������� ������Ʈ�� �׸��� �����ϱ� ���� Ŭ����
/// OIT �� ���� �ʿ��� ���۵��� �ִ�
/// ����, OIT ����� linked-list OIT
/// �̸� ���ؼ��� 
/// FragmentAndLinkBuffer, StartOffsetBuffer 
/// ���� �ʿ��ϴ�
/// �̶� FragmentAndLinkBuffer�� StructuredBuffer��
/// StartOffsetBuffer�� ByteAddressBuffer��
/// �� ���� ��� DX11���� ���Ӱ� ������� ���۵��̴�

#include "D3D11Define.h"

#include <queue>

struct FragmentAndLinkBuffer
{
	UINT uPixelColor;
	UINT uDepth;
	UINT uNext;
}; 

class OIT
{
public:
	OIT();
	~OIT();

public:
	
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);
	void OnResize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height);


private:
	UINT m_Width;
	UINT m_Height;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_StructureBuffer;
	Microsoft::WRL::ComPtr <ID3D11UnorderedAccessView> m_StructureedUAV;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> m_StructureedSRV;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_ByteAddressBuffer;
	Microsoft::WRL::ComPtr <ID3D11UnorderedAccessView> m_ByteAddressUAV;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> m_ByteAddressSRV;

};

