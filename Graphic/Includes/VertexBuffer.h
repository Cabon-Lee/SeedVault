#ifndef VertexBuffer_h__
#define VertexBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

template<class T>
class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT stride = sizeof(T);
	UINT vertexCount = 0;

public:
	VertexBuffer() {}

	//수제 복사 생성자
	VertexBuffer(const VertexBuffer<T>& rhs)
	{
		this->buffer = rhs.buffer;
		this->vertexCount = rhs.vertexCount;
		this->stride = rhs.stride;
	}

	//연산자 오버로딩
	VertexBuffer<T>& operator=(const VertexBuffer<T>& a)
	{
		this->buffer = a.buffer;
		this->vertexCount = a.vertexCount;
		this->stride = a.stride;
		return *this;
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetPtr()
	{
		return buffer;
	}

	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	UINT VertexCount() const
	{
		return this->vertexCount;
	}

	const UINT Stride() const
	{
		return this->stride;
	}

	const UINT* StridePtr() const
	{
		return &this->stride;
	}

	template<typename T>
	HRESULT Initialize(ID3D11Device* device, T* data, UINT vertexCount)
	{
		//메모리 누수 방지
		if (buffer.Get() != nullptr)
			buffer.Reset();

		this->vertexCount = vertexCount;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = stride * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->buffer.GetAddressOf());
		return hr;
	}

	template<typename T>
	HRESULT InitializeDynamic(ID3D11Device* device, T* data, UINT vertexCount)
	{
		//메모리 누수 방지
		if (buffer.Get() != nullptr)
			buffer.Reset();

		this->vertexCount = vertexCount;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = stride * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->buffer.GetAddressOf());
		return hr;
	}
};

#endif // VertexBuffer_h__
