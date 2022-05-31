#pragma once
#include <wrl/client.h>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
//#include "NodeData.h"

struct SpriteData;

class SpriteQuad
{
public:
	SpriteQuad();
	~SpriteQuad();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;

	UINT m_Stride;
	UINT m_IndexCount;

	/// 디버깅용 테두리
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBufferEdge;
	
	UINT m_StrideEdge;

public:
	void SetBuffer(ID3D11Device* device);
	
	ID3D11Buffer* const* GetVertexAddressOf()
	{
		return m_VertexBuffer.GetAddressOf();
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer()
	{
		return m_VertexBuffer;
	}

	ID3D11Buffer* GetIndexBuffer()
	{
		return m_IndexBuffer.Get();
	}

	const UINT GetIndexCount()
	{
		return m_IndexCount;
	}

	const UINT* StridePtr() const
	{
		return &this->m_Stride;
	}

	ID3D11Buffer* const* GetVertexEdgeAddressOf()
	{
		return m_VertexBufferEdge.GetAddressOf();
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexEdgeBuffer()
	{
		return m_VertexBufferEdge;
	}

	const UINT* StrideEdgePtr() const
	{
		return &this->m_StrideEdge;
	}
};

