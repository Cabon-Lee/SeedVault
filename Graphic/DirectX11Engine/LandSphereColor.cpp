#define _USE_MATH_DEFINES
#include "LandSphereColor.h"

#include <Math.h>
#include <iterator>
#include <DirectXColors.h>
#include "COMException.h"
#include "Shaders.h"

bool LandSphereColor::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, float radius, UINT stackCount, UINT sliceCount, float scale)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;

	this->radius = radius;

	this->stackCount = stackCount;
	this->sliceCount = sliceCount;

	this->scale = XMFLOAT3(scale, scale, scale);

	//this->SetRotation(-DirectX::XM_PI, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, -DirectX::XM_PI / 2);
	if (!InitGeometry(DirectX::Colors::OliveDrab)) { return false; }

//#if _DEBUG //점, 면, 사각면의 노멀 비주얼라이징
	InsertVertexNormal();
	InsertFaceNormal();
	InsertSquareFaceNormal();

	InitVertexNormal();
	InitFaceNormal();
	InitSquareFaceNormal();

	insertballPosition();
//#endif

	return true;
}

void LandSphereColor::Destroy()
{
	{if (device) { delete (device); (device) = nullptr; }}
	{if (deviceContext) { delete (deviceContext); (deviceContext) = nullptr; }}
	{if (cb_vs_vertexshader) { delete (cb_vs_vertexshader); (cb_vs_vertexshader) = nullptr; }}
	{if (texture) { delete (texture); (texture) = nullptr; }}
	{if (vertex) { delete (vertex); (vertex) = nullptr; }}
	{if (indices) { delete (indices); (indices) = nullptr; }}
}

void LandSphereColor::Update(BoundingSphere& sh, XMFLOAT3 pos)
{

}

void LandSphereColor::Draw(const XMMATRIX& viewProjectionMatrix)
{
	this->cb_vs_vertexshader->data.wvpMatrix = worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix
	this->cb_vs_vertexshader->data.worldMatrix = worldMatrix;
	this->cb_vs_vertexshader->ApplyChanges();
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());

	this->deviceContext->IASetInputLayout(Shaders::colorNormalShader.GetInputLayout());
	this->deviceContext->VSSetShader(Shaders::colorNormalShader.GetVertexShader(), NULL, 0);
	this->deviceContext->PSSetShader(Shaders::colorNormalShader.GetPixelShader(), NULL, 0);

	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	this->deviceContext->DrawIndexed(this->indexBuffer.IndexCount(), 0, 0);

//#if _DEBUG //점, 면, 사각면의 노멀 비주얼라이징
	//DrawVertexNormal(worldMatrix, viewProjectionMatrix);
	//DrawFaceNormal(worldMatrix, viewProjectionMatrix);
	DrawSquareFaceNormal(worldMatrix, viewProjectionMatrix);
//#endif
}

bool LandSphereColor::InitGeometry(XMVECTORF32 Color)
{
	try
	{
		/// 버텍스 버퍼
		std::vector<Vertex::VertexColorNormal> SphereVector;

		Vertex::VertexColorNormal topVertex(0.0f, +radius, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		Vertex::VertexColorNormal bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

		topVertex.rgba = XMFLOAT4((const float*)Color);
		bottomVertex.rgba = XMFLOAT4((const float*)Color);

		SphereVector.push_back(topVertex);

		float thetaStep = M_PI / stackCount;
		float phiStep = 2.0f * M_PI / sliceCount;

		for (UINT i = 1; i <= stackCount - 1; i++)
		{
			float theta = i * thetaStep;
			for (UINT j = 0; j <= sliceCount; j++)
			{
				float phi = j * phiStep;

				DirectX::XMFLOAT3 p = XMFLOAT3
				(
					(this->radius * sinf(theta) * cosf(phi)),
					(this->radius * cosf(theta)),
					(this->radius * sinf(theta) * sinf(phi))
				);

				DirectX::XMFLOAT3 t = XMFLOAT3
				(
					(-this->radius * sinf(theta) * sinf(phi)),
					0.f,
					(this->radius * sinf(theta) * cosf(phi))
				);

				XMVECTOR tV = XMLoadFloat3(&t);
				XMStoreFloat3(&t, XMVector3Normalize(tV));

				XMVECTOR pV = XMLoadFloat3(&p);
				XMFLOAT3 n;
				XMStoreFloat3(&n, XMVector3Normalize(pV));

				Vertex::VertexColorNormal vertexColor(p.x, p.y, p.z, n.x, n.y, n.z, 0, 0, 0, 0);
				vertexColor.rgba = XMFLOAT4((const float*)Color);

				SphereVector.push_back(vertexColor);
			}
		}

		SphereVector.push_back(bottomVertex);

		vertex = new Vertex::VertexColorNormal[SphereVector.size()];
		UINT vertexCount = SphereVector.size();
		std::copy(SphereVector.begin(), SphereVector.end(), stdext::checked_array_iterator<Vertex::VertexColorNormal*>(vertex, vertexCount));

		//버텍스 버퍼 불러오기
		HRESULT hr = this->vertexBuffer.Initialize(this->device, vertex, vertexCount);
		COM_ERROR_IF_FAILED(hr, "버텍스 버퍼를 생성하는데 실패했습니다");

		/// 인덱스 버퍼
		std::vector<DWORD> indexTemp;

		/// 윗뚜껑부분은 포함하지 않는다
		//for (UINT i = 1; i <= sliceCount; i++)
		//{
		//	indexTemp.push_back(0);
		//	indexTemp.push_back(i + 1);
		//	indexTemp.push_back(i);
		//}

		/// 버텍스 노멀 시작점과 끝점을 위한 변수 
		Vertex::VertexColor vertexNormalStart[6];
		Vertex::VertexColor vertexNormalEnd[6];

		UINT baseIndex = 1;
		UINT ringVertexCount = sliceCount + 1;

		for (UINT i = 0; i < stackCount - 2; ++i)
		{
			for (UINT j = 0; j < sliceCount; ++j)
			{
				indexTemp.push_back(baseIndex + i * ringVertexCount + j);
				indexTemp.push_back(baseIndex + i * ringVertexCount + j + 1);
				indexTemp.push_back(baseIndex + (i + 1) * ringVertexCount + j);

				indexTemp.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				indexTemp.push_back(baseIndex + i * ringVertexCount + j + 1);
				indexTemp.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);

				///버텍스 노말의 시작점과 끝점을 차례대로 집어넣는다
				vertexNormalStart[0].pos.x = vertex[baseIndex + i * ringVertexCount + j].pos.x;
				vertexNormalStart[0].pos.y = vertex[baseIndex + i * ringVertexCount + j].pos.y;
				vertexNormalStart[0].pos.z = vertex[baseIndex + i * ringVertexCount + j].pos.z;

				vertexNormalStart[1].pos.x = vertex[baseIndex + i * ringVertexCount + j + 1].pos.x;
				vertexNormalStart[1].pos.y = vertex[baseIndex + i * ringVertexCount + j + 1].pos.y;
				vertexNormalStart[1].pos.z = vertex[baseIndex + i * ringVertexCount + j + 1].pos.z;

				vertexNormalStart[2].pos.x = vertex[baseIndex + (i + 1) * ringVertexCount + j].pos.x;
				vertexNormalStart[2].pos.y = vertex[baseIndex + (i + 1) * ringVertexCount + j].pos.y;
				vertexNormalStart[2].pos.z = vertex[baseIndex + (i + 1) * ringVertexCount + j].pos.z;

				vertexNormalStart[3].pos.x = vertex[baseIndex + (i + 1) * ringVertexCount + j].pos.x;
				vertexNormalStart[3].pos.y = vertex[baseIndex + (i + 1) * ringVertexCount + j].pos.y;
				vertexNormalStart[3].pos.z = vertex[baseIndex + (i + 1) * ringVertexCount + j].pos.z;

				vertexNormalStart[4].pos.x = vertex[baseIndex + i * ringVertexCount + j + 1].pos.x;
				vertexNormalStart[4].pos.y = vertex[baseIndex + i * ringVertexCount + j + 1].pos.y;
				vertexNormalStart[4].pos.z = vertex[baseIndex + i * ringVertexCount + j + 1].pos.z;

				vertexNormalStart[5].pos.x = vertex[baseIndex + (i + 1) * ringVertexCount + j + 1].pos.x;
				vertexNormalStart[5].pos.y = vertex[baseIndex + (i + 1) * ringVertexCount + j + 1].pos.y;
				vertexNormalStart[5].pos.z = vertex[baseIndex + (i + 1) * ringVertexCount + j + 1].pos.z;

				vertexNormalEnd[0].pos.x = vertex[baseIndex + i * ringVertexCount + j].pos.x + vertex[baseIndex + i * ringVertexCount + j].normal.x;
				vertexNormalEnd[0].pos.y = vertex[baseIndex + i * ringVertexCount + j].pos.y + vertex[baseIndex + i * ringVertexCount + j].normal.y;
				vertexNormalEnd[0].pos.z = vertex[baseIndex + i * ringVertexCount + j].pos.z + vertex[baseIndex + i * ringVertexCount + j].normal.z;

				vertexNormalEnd[1].pos.x = vertex[baseIndex + i * ringVertexCount + j + 1].pos.x + vertex[baseIndex + i * ringVertexCount + j + 1].normal.x;
				vertexNormalEnd[1].pos.y = vertex[baseIndex + i * ringVertexCount + j + 1].pos.y + vertex[baseIndex + i * ringVertexCount + j + 1].normal.y;
				vertexNormalEnd[1].pos.z = vertex[baseIndex + i * ringVertexCount + j + 1].pos.z + vertex[baseIndex + i * ringVertexCount + j + 1].normal.z;

				vertexNormalEnd[2].pos.x = vertex[baseIndex + (i + 1) * ringVertexCount + j].pos.x + vertex[baseIndex + (i + 1) * ringVertexCount + j].normal.x;
				vertexNormalEnd[2].pos.y = vertex[baseIndex + (i + 1) * ringVertexCount + j].pos.y + vertex[baseIndex + (i + 1) * ringVertexCount + j].normal.y;
				vertexNormalEnd[2].pos.z = vertex[baseIndex + (i + 1) * ringVertexCount + j].pos.z + vertex[baseIndex + (i + 1) * ringVertexCount + j].normal.z;

				vertexNormalEnd[3].pos.x = vertex[baseIndex + (i + 1) * ringVertexCount + j].pos.x + vertex[baseIndex + (i + 1) * ringVertexCount + j].normal.x;
				vertexNormalEnd[3].pos.y = vertex[baseIndex + (i + 1) * ringVertexCount + j].pos.y + vertex[baseIndex + (i + 1) * ringVertexCount + j].normal.y;
				vertexNormalEnd[3].pos.z = vertex[baseIndex + (i + 1) * ringVertexCount + j].pos.z + vertex[baseIndex + (i + 1) * ringVertexCount + j].normal.z;

				vertexNormalEnd[4].pos.x = vertex[baseIndex + i * ringVertexCount + j + 1].pos.x + vertex[baseIndex + i * ringVertexCount + j + 1].normal.x;
				vertexNormalEnd[4].pos.y = vertex[baseIndex + i * ringVertexCount + j + 1].pos.y + vertex[baseIndex + i * ringVertexCount + j + 1].normal.y;
				vertexNormalEnd[4].pos.z = vertex[baseIndex + i * ringVertexCount + j + 1].pos.z + vertex[baseIndex + i * ringVertexCount + j + 1].normal.z;

				vertexNormalEnd[5].pos.x = vertex[baseIndex + (i + 1) * ringVertexCount + j + 1].pos.x + vertex[baseIndex + (i + 1) * ringVertexCount + j + 1].normal.x;
				vertexNormalEnd[5].pos.y = vertex[baseIndex + (i + 1) * ringVertexCount + j + 1].pos.y + vertex[baseIndex + (i + 1) * ringVertexCount + j + 1].normal.y;
				vertexNormalEnd[5].pos.z = vertex[baseIndex + (i + 1) * ringVertexCount + j + 1].pos.z + vertex[baseIndex + (i + 1) * ringVertexCount + j + 1].normal.z;

				for (int i = 0; i < 6; ++i)
				{
					vertexNormalStart[i].rgba = XMFLOAT4((const float*)&DirectX::Colors::Crimson);
					vertexNormalEnd[i].rgba = XMFLOAT4((const float*)&DirectX::Colors::Crimson);

					//VNvertices.push_back(vertexNormalStart[i]);
					//VNvertices.push_back(vertexNormalEnd[i]);
				}
			}
		}

		UINT southPoleIndex = SphereVector.size() - 1;
		baseIndex = southPoleIndex - ringVertexCount;

		/// 아래뚜껑부분도 포함하지 않는다
		//for (UINT i = 0; i < sliceCount; ++i)
		//{
		//	indexTemp.push_back(southPoleIndex);
		//	indexTemp.push_back(baseIndex + i);
		//	indexTemp.push_back(baseIndex + i + 1);
		//}

		indices = new DWORD[indexTemp.size()];
		UINT indexCount = indexTemp.size();
		std::copy(indexTemp.begin(), indexTemp.end(), stdext::checked_array_iterator<DWORD*>(indices, indexCount));

		//인덱스 버퍼 불러오기
		hr = this->indexBuffer.Initialize(this->device, indices, indexCount);
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");

	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

bool LandSphereColor::InsertVertexNormal()
{
	Vertex::VertexColor vertexNormalStart;
	Vertex::VertexColor vertexNormalEnd;

	for (int i = 0; i < _msize(indices) / sizeof(DWORD); ++i)
	{
		vertexNormalStart.pos.x = vertex[indices[i]].pos.x;
		vertexNormalStart.pos.y = vertex[indices[i]].pos.y;
		vertexNormalStart.pos.z = vertex[indices[i]].pos.z;

		vertexNormalEnd.pos.x = vertex[indices[i]].pos.x + vertex[indices[i]].normal.x * 0.5;
		vertexNormalEnd.pos.y = vertex[indices[i]].pos.y + vertex[indices[i]].normal.y * 0.5;
		vertexNormalEnd.pos.z = vertex[indices[i]].pos.z + vertex[indices[i]].normal.z * 0.5;

		vertexNormalStart.rgba = XMFLOAT4((const float*)&DirectX::Colors::Crimson);
		vertexNormalEnd.rgba = XMFLOAT4((const float*)&DirectX::Colors::Crimson);

		VNvertices.push_back(vertexNormalStart);
		VNvertices.push_back(vertexNormalEnd);

		vertexNormalStart.pos = { 0,0,0 };
		vertexNormalStart.rgba = { 0,0,0,0 };
		vertexNormalEnd.pos = { 0,0,0 };
		vertexNormalEnd.rgba = { 0,0,0,0 };
	}

	// 버텍스 노말값 인덱스
	for (DWORD i = 0; i < VNvertices.size(); ++i)
	{
		VNindices.push_back(i);
	}

	return true;
}

bool LandSphereColor::InsertFaceNormal()
{
	//6개씩 묶는다
	//시작점 - 노말점
	//시작점 - 노말점
	//시작점 - 노말점

	Vertex::VertexColor vertexNormalStartTemp[3];
	Vertex::VertexColor vertexNormalEndTemp[3];

	for (int i = 0; i < VNindices.size(); ++i)
	{
		if (i % 6 == 0)
		{
			vertexNormalStartTemp[0].pos.x = VNvertices[i].pos.x;
			vertexNormalStartTemp[0].pos.y = VNvertices[i].pos.y;
			vertexNormalStartTemp[0].pos.z = VNvertices[i].pos.z;
		}
		else if (i % 6 == 1)
		{
			vertexNormalEndTemp[0].pos.x = VNvertices[i].pos.x;
			vertexNormalEndTemp[0].pos.y = VNvertices[i].pos.y;
			vertexNormalEndTemp[0].pos.z = VNvertices[i].pos.z;
		}
		else if (i % 6 == 2)
		{
			vertexNormalStartTemp[1].pos.x = VNvertices[i].pos.x;
			vertexNormalStartTemp[1].pos.y = VNvertices[i].pos.y;
			vertexNormalStartTemp[1].pos.z = VNvertices[i].pos.z;
		}
		else if (i % 6 == 3)
		{
			vertexNormalEndTemp[1].pos.x = VNvertices[i].pos.x;
			vertexNormalEndTemp[1].pos.y = VNvertices[i].pos.y;
			vertexNormalEndTemp[1].pos.z = VNvertices[i].pos.z;
		}
		else if (i % 6 == 4)
		{
			vertexNormalStartTemp[2].pos.x = VNvertices[i].pos.x;
			vertexNormalStartTemp[2].pos.y = VNvertices[i].pos.y;
			vertexNormalStartTemp[2].pos.z = VNvertices[i].pos.z;
		}
		else if (i % 6 == 5)
		{
			vertexNormalEndTemp[2].pos.x = VNvertices[i].pos.x;
			vertexNormalEndTemp[2].pos.y = VNvertices[i].pos.y;
			vertexNormalEndTemp[2].pos.z = VNvertices[i].pos.z;
		}

		//점 6개가 다채워졌을 때
		if (i % 6 == 5)
		{
			Vertex::VertexColor faceNormalStart;
			Vertex::VertexColor faceNormalEnd;

			faceNormalStart.pos.x = (vertexNormalStartTemp[0].pos.x + vertexNormalStartTemp[1].pos.x + vertexNormalStartTemp[2].pos.x) / 3;
			faceNormalStart.pos.y = (vertexNormalStartTemp[0].pos.y + vertexNormalStartTemp[1].pos.y + vertexNormalStartTemp[2].pos.y) / 3;
			faceNormalStart.pos.z = (vertexNormalStartTemp[0].pos.z + vertexNormalStartTemp[1].pos.z + vertexNormalStartTemp[2].pos.z) / 3;

			faceNormalEnd.pos.x = (vertexNormalEndTemp[0].pos.x + vertexNormalEndTemp[1].pos.x + vertexNormalEndTemp[2].pos.x) / 3;
			faceNormalEnd.pos.y = (vertexNormalEndTemp[0].pos.y + vertexNormalEndTemp[1].pos.y + vertexNormalEndTemp[2].pos.y) / 3;
			faceNormalEnd.pos.z = (vertexNormalEndTemp[0].pos.z + vertexNormalEndTemp[1].pos.z + vertexNormalEndTemp[2].pos.z) / 3;

			faceNormalStart.rgba = XMFLOAT4((const float*)&DirectX::Colors::DodgerBlue);
			faceNormalEnd.rgba = XMFLOAT4((const float*)&DirectX::Colors::DodgerBlue);

			FNvertices.push_back(faceNormalStart);
			FNvertices.push_back(faceNormalEnd);

			for (int j = 0; j < 3; ++j)
			{
				vertexNormalStartTemp[j].pos = { 0,0,0 };
				vertexNormalStartTemp[j].rgba = { 0,0,0,0 };
				vertexNormalEndTemp[j].pos = { 0,0,0 };
				vertexNormalEndTemp[j].rgba = { 0,0,0,0 };
			}
		}
	}

	// 페이스 노말값 인덱스
	for (DWORD i = 0; i < FNvertices.size(); ++i)
	{
		FNindices.push_back(i);
	}

	return true;
}

bool LandSphereColor::InsertSquareFaceNormal()
{
	Vertex::VertexColor faceNormalStartTemp[2];
	Vertex::VertexColor faceNormalEndTemp[2];

	//4개씩 묶는다
	//시작점 - 노말점
	//시작점 - 노말점

	for (int i = 0; i < FNindices.size(); ++i)
	{
		//if (i < 60 * 4)
		{
			if (i % 4 == 0)
			{
				faceNormalStartTemp[0].pos.x = FNvertices[i].pos.x;
				faceNormalStartTemp[0].pos.y = FNvertices[i].pos.y;
				faceNormalStartTemp[0].pos.z = FNvertices[i].pos.z;
			}
			else if (i % 4 == 1)
			{
				faceNormalEndTemp[0].pos.x = FNvertices[i].pos.x;
				faceNormalEndTemp[0].pos.y = FNvertices[i].pos.y;
				faceNormalEndTemp[0].pos.z = FNvertices[i].pos.z;
			}
			else if (i % 4 == 2)
			{
				faceNormalStartTemp[1].pos.x = FNvertices[i].pos.x;
				faceNormalStartTemp[1].pos.y = FNvertices[i].pos.y;
				faceNormalStartTemp[1].pos.z = FNvertices[i].pos.z;
			}
			else if (i % 4 == 3)
			{
				faceNormalEndTemp[1].pos.x = FNvertices[i].pos.x;
				faceNormalEndTemp[1].pos.y = FNvertices[i].pos.y;
				faceNormalEndTemp[1].pos.z = FNvertices[i].pos.z;
			}

			if (i % 4 == 3)
			{
				Vertex::VertexColor squareFaceNormalStart;
				Vertex::VertexColor squareFaceNormalEnd;

				squareFaceNormalStart.pos.x = (faceNormalStartTemp[0].pos.x + faceNormalStartTemp[1].pos.x) / 2;
				squareFaceNormalStart.pos.y = (faceNormalStartTemp[0].pos.y + faceNormalStartTemp[1].pos.y) / 2;
				squareFaceNormalStart.pos.z = (faceNormalStartTemp[0].pos.z + faceNormalStartTemp[1].pos.z) / 2;

				squareFaceNormalEnd.pos.x = (faceNormalEndTemp[0].pos.x + faceNormalEndTemp[1].pos.x) / 2;
				squareFaceNormalEnd.pos.y = (faceNormalEndTemp[0].pos.y + faceNormalEndTemp[1].pos.y) / 2;
				squareFaceNormalEnd.pos.z = (faceNormalEndTemp[0].pos.z + faceNormalEndTemp[1].pos.z) / 2;

				squareFaceNormalStart.rgba = XMFLOAT4((const float*)&DirectX::Colors::Gold);
				squareFaceNormalEnd.rgba = XMFLOAT4((const float*)&DirectX::Colors::Gold);

				SFNvertices.push_back(squareFaceNormalStart);
				SFNvertices.push_back(squareFaceNormalEnd);

				for (int j = 0; j < 2; ++j)
				{
					faceNormalStartTemp[j].pos = { 0,0,0 };
					faceNormalStartTemp[j].rgba = { 0,0,0,0 };
					faceNormalEndTemp[j].pos = { 0,0,0 };
					faceNormalEndTemp[j].rgba = { 0,0,0,0 };
				}
			}
		}
	}

	// 스퀘어 페이스 노말값 인덱스
	for (DWORD i = 0; i < SFNvertices.size(); ++i)
	{
		SFNindices.push_back(i);
	}

	return true;
}

bool LandSphereColor::InitVertexNormal()
{
	//버텍스버퍼 벡터와 인덱스 버퍼 벡터를 배열로 변환
	Vertex::VertexColor* vertex = new Vertex::VertexColor[VNvertices.size()];
	UINT vertexCount = VNvertices.size();
	std::copy(VNvertices.begin(), VNvertices.end(), stdext::checked_array_iterator<Vertex::VertexColor*>(vertex, vertexCount));

	//버텍스 버퍼 불러오기
	HRESULT hr = this->VNvertexBuffer.Initialize(this->device, vertex, vertexCount);
	COM_ERROR_IF_FAILED(hr, "버텍스 버퍼를 생성하는데 실패했습니다");

	DWORD* indices = new DWORD[VNindices.size()];
	UINT indexCount = VNindices.size();
	std::copy(VNindices.begin(), VNindices.end(), stdext::checked_array_iterator<DWORD*>(indices, indexCount));

	//인덱스 버퍼 불러오기
	hr = this->VNindexBuffer.Initialize(this->device, indices, indexCount);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");

	return true;
}

bool LandSphereColor::InitFaceNormal()
{
	//버텍스버퍼 벡터와 인덱스 버퍼 벡터를 배열로 변환
	Vertex::VertexColor* vertex = new Vertex::VertexColor[FNvertices.size()];
	UINT vertexCount = FNvertices.size();
	std::copy(FNvertices.begin(), FNvertices.end(), stdext::checked_array_iterator<Vertex::VertexColor*>(vertex, vertexCount));

	//버텍스 버퍼 불러오기
	HRESULT hr = this->FNvertexBuffer.Initialize(this->device, vertex, vertexCount);
	COM_ERROR_IF_FAILED(hr, "버텍스 버퍼를 생성하는데 실패했습니다");

	DWORD* indices = new DWORD[FNindices.size()];
	UINT indexCount = FNindices.size();
	std::copy(FNindices.begin(), FNindices.end(), stdext::checked_array_iterator<DWORD*>(indices, indexCount));

	//인덱스 버퍼 불러오기
	hr = this->FNindexBuffer.Initialize(this->device, indices, indexCount);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");

	return true;
}

bool LandSphereColor::InitSquareFaceNormal()
{
	//버텍스버퍼 벡터와 인덱스 버퍼 벡터를 배열로 변환
	Vertex::VertexColor* vertex = new Vertex::VertexColor[SFNvertices.size()];
	UINT vertexCount = SFNvertices.size();
	std::copy(SFNvertices.begin(), SFNvertices.end(), stdext::checked_array_iterator<Vertex::VertexColor*>(vertex, vertexCount));

	//버텍스 버퍼 불러오기
	HRESULT hr = this->SFNvertexBuffer.Initialize(this->device, vertex, vertexCount);
	COM_ERROR_IF_FAILED(hr, "버텍스 버퍼를 생성하는데 실패했습니다");

	DWORD* indices = new DWORD[SFNindices.size()];
	UINT indexCount = SFNindices.size();
	std::copy(SFNindices.begin(), SFNindices.end(), stdext::checked_array_iterator<DWORD*>(indices, indexCount));

	//인덱스 버퍼 불러오기
	hr = this->SFNindexBuffer.Initialize(this->device, indices, indexCount);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");

	return true;
}

void LandSphereColor::DrawVertexNormal(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)
{
	/// WVP TM등을 셋팅
	// Set constants
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());
	this->cb_vs_vertexshader->data.wvpMatrix = worldMatrix * viewProjectionMatrix;
	this->cb_vs_vertexshader->ApplyChanges();

	// 입력 배치 객체 셋팅
	this->deviceContext->IASetInputLayout(Shaders::colorShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	this->deviceContext->VSSetShader(Shaders::colorShader.GetVertexShader(), NULL, 0);
	this->deviceContext->PSSetShader(Shaders::colorShader.GetPixelShader(), NULL, 0);

	// 인덱스버퍼와 버텍스버퍼 셋팅
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, VNvertexBuffer.GetAddressOf(), VNvertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(VNindexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(this->VNindexBuffer.IndexCount(), 0, 0);
}

void LandSphereColor::DrawFaceNormal(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)
{
	/// WVP TM등을 셋팅
	// Set constants
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());
	this->cb_vs_vertexshader->data.wvpMatrix = worldMatrix * viewProjectionMatrix;
	this->cb_vs_vertexshader->ApplyChanges();

	// 입력 배치 객체 셋팅
	this->deviceContext->IASetInputLayout(Shaders::colorShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	this->deviceContext->VSSetShader(Shaders::colorShader.GetVertexShader(), NULL, 0);
	this->deviceContext->PSSetShader(Shaders::colorShader.GetPixelShader(), NULL, 0);

	// 인덱스버퍼와 버텍스버퍼 셋팅
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, FNvertexBuffer.GetAddressOf(), FNvertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(FNindexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(this->FNindexBuffer.IndexCount(), 0, 0);
}

void LandSphereColor::DrawSquareFaceNormal(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)
{
	/// WVP TM등을 셋팅
	// Set constants
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());
	this->cb_vs_vertexshader->data.wvpMatrix = worldMatrix * viewProjectionMatrix;
	this->cb_vs_vertexshader->ApplyChanges();

	// 입력 배치 객체 셋팅
	this->deviceContext->IASetInputLayout(Shaders::colorShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	this->deviceContext->VSSetShader(Shaders::colorShader.GetVertexShader(), NULL, 0);
	this->deviceContext->PSSetShader(Shaders::colorShader.GetPixelShader(), NULL, 0);

	// 인덱스버퍼와 버텍스버퍼 셋팅
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, SFNvertexBuffer.GetAddressOf(), SFNvertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(SFNindexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(this->SFNindexBuffer.IndexCount(), 0, 0);
}

void LandSphereColor::insertballPosition()
{
	//for (int i = 0; i < SFNindices.size(); ++i)
	//{
	//	if (i % 2 == 1)
	//	{
	//		for (int j = 0; j < 29; ++j)
	//		{
	//			for (int k = 0; k < 50; ++k)
	//			{
	//				normalPos[j].push_back(SFNvertices[i].pos);
	//			}
	//		}
	//	}
	//}

	for (int i = 0; i < SFNindices.size(); ++i)
	{
		if (i % 2 == 1)
		{
			int index = 0;
			if (i % 58 == 0)
			{
				normalPos[index].push_back(SFNvertices[i].pos);
			}
		}
	}
}

void LandSphereColor::UpdateMatrix()
{
	this->worldMatrix = XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}
