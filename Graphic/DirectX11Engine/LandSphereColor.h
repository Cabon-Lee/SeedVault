#pragma once
#include "VertexHelper.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "GameObject3D.h"
#include <SimpleMath.h>
#include <PrimitiveBatch.h>

#define SAFE_DELETE(p)			{if(p) { delete (p); (p) = nullptr;}}
#define SAFE_DELETE_ARRAY(p)	{if(p) { delete[] (p); (p) = nullptr;}}
#define SAFE_RELEASE(p)			{if(p) { (p)->Release(); (p) = nullptr;}}
#define SAFE_CLASS(p)			{if(p) { (p)->Release(); delete (p); (p) = nullptr;}}

using namespace DirectX;

class LandSphereColor : public GameObject3D
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, float radius, UINT stackCount, UINT sliceCount, float sacle);
	void Destroy();
	void Update(BoundingSphere& sh, XMFLOAT3 pos);
	void Draw(const XMMATRIX& viewProjectionMatrix);

private:
	bool InitGeometry(XMVECTORF32 Color);
	void UpdateMatrix();

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;

	VertexBuffer<Vertex::VertexColorNormal> vertexBuffer;
	IndexBuffer indexBuffer;

	Vertex::VertexColorNormal* vertex;
	DWORD* indices;

	
	bool InsertVertexNormal();
	bool InsertFaceNormal();
	bool InsertSquareFaceNormal();

	bool InitVertexNormal();
	bool InitFaceNormal();
	bool InitSquareFaceNormal();

	void DrawVertexNormal(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);
	void DrawFaceNormal(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);
	void DrawSquareFaceNormal(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);

	// ���ߺ��ͷ� ������ �븻 ��ǥ�� �ִ´� 50 x 29
	void insertballPosition();

	// vertex �븻 ���־����¡�� ���ؽ� ���ۿ� �ε��� ����
	std::vector<Vertex::VertexColor> VNvertices;
	std::vector<DWORD> VNindices;
	VertexBuffer<Vertex::VertexColor> VNvertexBuffer;
	IndexBuffer VNindexBuffer;

	// face(�ﰢ��) �븻 ���־����¡�� ���ؽ� ���ۿ� �ε��� ����
	std::vector<Vertex::VertexColor> FNvertices;
	std::vector<DWORD> FNindices;
	VertexBuffer<Vertex::VertexColor> FNvertexBuffer;
	IndexBuffer FNindexBuffer;

	// face(�簢��) �븻 ���־����¡�� ���ؽ� ���ۿ� �ε��� ����
	std::vector<Vertex::VertexColor> SFNvertices;
	std::vector<DWORD> SFNindices;
	VertexBuffer<Vertex::VertexColor> SFNvertexBuffer;
	IndexBuffer SFNindexBuffer;

	// �����ֳ� �����۵��� �ö� ��ġ
	std::vector<std::vector<XMFLOAT3>> normalPos;

	XMMATRIX worldMatrix = XMMatrixIdentity();

	float radius = 10.f;
	UINT stackCount = 20;	// ����
	UINT sliceCount = 20;	// ����

};

