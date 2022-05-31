#pragma once

/// 2022.03.24
/// 불투명한 오브젝트를 그리고 관리하기 위한 클래스
/// OIT 를 위해 필요한 버퍼들이 있다
/// 먼저, OIT 방식은 linked-list OIT
/// 이를 위해서는 
/// FragmentAndLinkBuffer, StartOffsetBuffer 
/// 등이 필요하다
/// 이때 FragmentAndLinkBuffer는 StructuredBuffer고
/// StartOffsetBuffer는 ByteAddressBuffer다
/// 두 버퍼 모두 DX11에서 새롭게 만들어진 버퍼들이다

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

