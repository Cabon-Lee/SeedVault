#pragma once

/// 기즈모에 이어 그리드를 만들려다보니 겹치는 부분이 많아 하나로 묶을 상위 클래스를 만들게 되었다
/// 그 이유는 나중에 또 다른 디버깅용 클래스가 필요할 것이라고 생각했기 때문
/// 또, 함수는 한번만 만들어서 돌려쓰고 싶었다

#include "D3D11Define.h"

struct NodeData;
struct ID3D11PixelShader;
class VertexShader;
class PixelShader;

class IHelperClass
{
public:
	IHelperClass() 
		: m_VertexBuffer(nullptr)
		, m_IndexBuffer(nullptr)
		, m_Stride(0)
		, m_IndexCount(0)
	{};
	~IHelperClass() {};

	virtual void BuildHelper(ID3D11Device* pDevice) abstract;
	virtual void HelperRender(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader,
		const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj) abstract;

	virtual void HelperRender(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
		std::shared_ptr<VertexShader> pVertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader,
		const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj)
	{
		// 구현 없음
	};

	int GetStride() { return m_Stride; };
	
	const UINT* GetStridePtr() 
	{ 
		m_Stride_UINT = static_cast<UINT>(m_Stride);
		return &m_Stride_UINT;
	};

	int GetIndexCount() { return m_IndexCount; };

	ID3D11Buffer* GetIndexBuffer()const
	{
		return m_IndexBuffer.Get();
	}

	ID3D11Buffer* const* GetAddressOfVertexBuffer()const
	{
		return m_VertexBuffer.GetAddressOf();
	}

protected:
	// 실제 메쉬 정보를 가지고 있는 구조체
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
	int m_Stride;
	UINT m_Stride_UINT;
	int m_IndexCount;

};
