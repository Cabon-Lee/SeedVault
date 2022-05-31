#pragma once

#include "D3D11Define.h"

class VertexShader;
class PixelShader;

class ModelMesh;

class SkyBoxCube
{
public:
	SkyBoxCube();
	~SkyBoxCube();

public:
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);
	void DrawSkyBox(
		Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext,
		Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> pSkyImage,
		const DirectX::XMFLOAT3& eyePos, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj,
		std::shared_ptr<VertexShader> pVertexShader, std::shared_ptr<PixelShader> pPixelShader);

	Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer>& GetIndexBuffer();
	UINT GetStride() { return m_Stride; }
	UINT GetIndexCount() { return m_IndexCount; }
	unsigned int m_SkyBoxTextureIndex;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
	UINT m_Stride;
	UINT m_IndexCount;
};

