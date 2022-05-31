#pragma once

/// ����� �̾� �׸��带 ������ٺ��� ��ġ�� �κ��� ���� �ϳ��� ���� ���� Ŭ������ ����� �Ǿ���
/// �� ������ ���߿� �� �ٸ� ������ Ŭ������ �ʿ��� ���̶�� �����߱� ����
/// ��, �Լ��� �ѹ��� ���� �������� �;���

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
		// ���� ����
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
	// ���� �޽� ������ ������ �ִ� ����ü
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
	int m_Stride;
	UINT m_Stride_UINT;
	int m_IndexCount;

};
