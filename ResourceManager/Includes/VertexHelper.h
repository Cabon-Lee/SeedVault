#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "d3dx11effect.h"

using namespace DirectX;

enum class VertexType
{
	Type_Vertex2D,
	Type_VertexPos,
	Type_Vertex,
	Type_VertexColor,
	Type_VertexColorNormal,
	Type_VertexSkinned,
};

namespace Vertex
{
	// 시리얼라이즈용 버텍스
	struct SerializerVertex
	{
		SerializerVertex()
			: pos(0, 0, 0), texCoord(0, 0), normal(0, 0, 0), weights(0, 0, 0), rgba(0, 0, 0, 0), tangent(0, 0, 0, 0)
		{
			boneIndices[0] = 0;
			boneIndices[1] = 0;
			boneIndices[2] = 0;
			boneIndices[3] = 0;
		};

		SerializerVertex(float x, float y, float z,
			float nx, float ny, float nz,
			float u, float v,
			float w1, float w2, float w3,
			float r, float g, float b, float a,
			float tx, float ty, float tz, float tw,
			BYTE indx1, BYTE indx2, BYTE indx3, BYTE indx4)
			: pos(x, y, z), normal(nx, ny, nz), texCoord(u, v), weights(w1, w2, w3), rgba(r, g, b, a), 
			tangent(tx, ty, tz, tw)
		{
			boneIndices[0] = indx1;
			boneIndices[1] = indx2;
			boneIndices[2] = indx3;
			boneIndices[3] = indx4;
		};

		XMFLOAT3 pos;
		XMFLOAT4 rgba;
		XMFLOAT2 texCoord;
		XMFLOAT3 normal;
		XMFLOAT4 tangent;
		XMFLOAT3 weights;
		UINT boneIndices[4];
	};

	struct Vertex2D
	{
		Vertex2D()
			: pos(0, 0, 0), texCoord(0, 0)
		{}

		Vertex2D(float x, float y, float z, float u, float v)
			: pos(x, y, z), texCoord(u, v) {}

		XMFLOAT3 pos;
		XMFLOAT2 texCoord;
	};

	struct VertexPos
	{
		VertexPos()
			: pos(0, 0, 0)
		{}

		VertexPos(float x, float y, float z)
			: pos(x, y, z) {}
		XMFLOAT3 pos;
	};

	struct VertexPosNormal
	{
		VertexPosNormal()
			: pos(0, 0, 0)
			, normal(0, 0, 0)
		{}

		VertexPosNormal(float x, float y, float z, float nx, float ny, float nz)
			: pos(x, y, z) 
			, normal(nx, ny, nz)
		{}
		XMFLOAT3 pos;
		XMFLOAT3 normal;
	};


	struct VertexColor
	{
		VertexColor()
			: pos(0, 0, 0), rgba(0, 0, 0, 0)
		{ }
		VertexColor(float x, float y, float z,
			float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
			: pos(x, y, z), rgba(r, g, b, a) {}

		VertexColor(XMFLOAT3 _pos, XMFLOAT4 _color) :
			pos(_pos), rgba(_color) {};

		XMFLOAT3 pos;
		XMFLOAT4 rgba;
	};

	struct VertexColorNormal
	{
		VertexColorNormal()
			: pos(0, 0, 0), normal(0, 0, 0), rgba(0, 0, 0, 0)
		{}
		VertexColorNormal(float x, float y, float z,
			float nx, float ny, float nz,
			float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
			: pos(x, y, z), normal(nx, ny, nz), rgba(r, g, b, a) {}

		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT4 rgba;
	};

	struct VertexColorTexture
	{
		VertexColorTexture()
			: pos(0, 0, 0), texture(0, 0), rgba(0, 0, 0, 0)
		{}
		VertexColorTexture(float x, float y, float z,
			float u, float v,
			float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
			: pos(x, y, z), texture(u, v), rgba(r, g, b, a) {}

		XMFLOAT3 pos;
		XMFLOAT2 texture;
		XMFLOAT4 rgba;
	};


	struct Vertex
	{
		Vertex()
			: pos(0, 0, 0), normal(0, 0, 0), texCoord(0, 0), tangent(0, 0, 0, 0)
		{}

		Vertex(float x, float y, float z,
			float nx, float ny, float nz,
			float u, float v,
			float tx, float ty, float tz)
			: pos(x, y, z), normal(nx, ny, nz), texCoord(u, v), tangent(tx, ty, tz, 1.0f) {}

		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
		// 추가
		XMFLOAT4 tangent;
	};

	struct VertexTangentU
	{
		VertexTangentU()
			:Position(0.0f, 0.0f, 0.0f), Normal(0.0f, 0.0f, 0.0f), TangentU(0.0f, 0.0f, 0.0f), texCoord(0.0f, 0.0f) {}
		VertexTangentU(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
			: Position(p), Normal(n), TangentU(t), texCoord(uv) {}
		VertexTangentU(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px, py, pz), Normal(nx, ny, nz),	TangentU(tx, ty, tz), texCoord(u, v) {}

		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 TangentU;
		XMFLOAT2 texCoord;
	};

	struct VertexSkinned
	{
		VertexSkinned()
			: pos(0, 0, 0), texCoord(0, 0), normal(0, 0, 0), weights()
		{
			boneIndices[0] = 0;
			boneIndices[1] = 0;
			boneIndices[2] = 0;
			boneIndices[3] = 0;
		};

		VertexSkinned(float x, float y, float z,
			float nx, float ny, float nz,
			float u, float v,
			float w1, float w2, float w3,
			BYTE indx1, BYTE indx2, BYTE indx3, BYTE indx4)
			: pos(x, y, z), normal(nx, ny, nz), texCoord(u, v), weights()
		{
			boneIndices[0] = indx1;
			boneIndices[1] = indx2;
			boneIndices[2] = indx3;
			boneIndices[3] = indx4;
		};

		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
		FLOAT weights[4];
		UINT boneIndices[4];
	};

	struct VertexNormalSkinned
	{
		VertexNormalSkinned()
			: pos(0, 0, 0), normal(0, 0, 0), texCoord(0, 0), tangent(0, 0, 0, 0), weights()
		{
			boneIndices[0] = 0;
			boneIndices[1] = 0;
			boneIndices[2] = 0;
			boneIndices[3] = 0;
		};

		VertexNormalSkinned(float x, float y, float z,
			float nx, float ny, float nz,
			float u, float v,
			float w1, float w2, float w3,
			BYTE indx1, BYTE indx2, BYTE indx3, BYTE indx4)
			: pos(x, y, z), normal(nx, ny, nz), texCoord(u, v), tangent(0, 0, 0, 0), weights()
		{
			boneIndices[0] = indx1;
			boneIndices[1] = indx2;
			boneIndices[2] = indx3;
			boneIndices[3] = indx4;
		};

		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
		XMFLOAT4 tangent;
		FLOAT weights[4];
		UINT boneIndices[4];
	};

	struct VertexPosNormalTex
	{
		VertexPosNormalTex()
			: pos(0, 0, 0), normal(0, 0, 0), tex(0, 0) {}
		VertexPosNormalTex(
			float x, float y, float z,
			float nx, float ny, float nz,
			float u, float v)
			: pos(x, y, z), normal(nx, ny, nz), tex(u, v) {}
			
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 tex;
	};

}

class InputLayoutDesc
{
public:
	static const D3D11_INPUT_ELEMENT_DESC layoutVertex2D[2];
	static const D3D11_INPUT_ELEMENT_DESC layoutVertexPos[1];
	static const D3D11_INPUT_ELEMENT_DESC layoutVertexColor[2];
	static const D3D11_INPUT_ELEMENT_DESC layoutVertexColorNormal[3];
	static const D3D11_INPUT_ELEMENT_DESC layoutVertexColorTexture[3];
	static const D3D11_INPUT_ELEMENT_DESC layoutVertex[3];
	static const D3D11_INPUT_ELEMENT_DESC layoutVertexTangentU[4];
	static const D3D11_INPUT_ELEMENT_DESC layoutVertexSkinned[5];

	static const UINT numElementsLayoutVertex2D = ARRAYSIZE(layoutVertex2D);
	static const UINT numElementsLayoutVertexPos = ARRAYSIZE(layoutVertexPos);
	static const UINT numElementsLayoutVertexColor = ARRAYSIZE(layoutVertexColor);
	static const UINT numElementsLayoutVertexColorNormal = ARRAYSIZE(layoutVertexColorNormal);
	static const UINT numElementsLayoutVertexColorTexture = ARRAYSIZE(layoutVertexColorTexture);
	static const UINT numElementsLayoutVertex = ARRAYSIZE(layoutVertex);
	static const UINT numElementsLayoutVertexTangentU = ARRAYSIZE(layoutVertexTangentU);
	static const UINT numElementsLayoutVertexSkinned = ARRAYSIZE(layoutVertexSkinned);
};

class InputLayouts
{
public:
	static void Init(ID3D11Device* device);
	static void Destroy();

	static ID3D11InputLayout* Vertex2D;
	static ID3D11InputLayout* VertexPos;
	static ID3D11InputLayout* VertexColor;
	static ID3D11InputLayout* VertexColorNormal;
	static ID3D11InputLayout* VertexColorTexture;
	static ID3D11InputLayout* VertexPosNormal;
	static ID3D11InputLayout* Vertex;
	static ID3D11InputLayout* VertexTangentU;
	static ID3D11InputLayout* VertexSkinned;
};