#include "IBL.h"

#include "CubeMapRenderTarget.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"

#include "Shaders.h"
#include "SkyBoxCube.h"
#include "Quad.h"

#include "ConstantBufferManager.h"
#include "RasterizerState.h"

#include "TextureCapture.h"

#include "StringHelper.h"
#include "COMException.h"
#include "ErrorLogger.h"


ReflectionProbe::ReflectionProbe()
	: m_pEnvironmentMap(nullptr)
	, m_pIrrdianceMap(nullptr)
	, m_pPreFilterMap(nullptr)
	, m_BakingTimer(0)
{
	m_BakedMapIndex[0] = 0; m_BakedMapIndex[1] = 0; m_BakedMapIndex[2] = 0;
}

ReflectionProbe::~ReflectionProbe()
{
	m_pEnvironmentMap.reset();
	m_pIrrdianceMap.reset();
	m_pPreFilterMap.reset();
	m_pEnviormentTexture.Reset();

	m_Cube.reset();
	m_Sphere.reset();
}

void ReflectionProbe::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	BuildHelper(pDevice);

	m_ViewTM_Arr[0] = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f });
	m_ViewTM_Arr[1] = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f });
	m_ViewTM_Arr[2] = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f });
	m_ViewTM_Arr[3] = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f });
	m_ViewTM_Arr[4] = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f });
	m_ViewTM_Arr[5] = DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, 0.0f });

	m_ProjTM = DirectX::SimpleMath::Matrix::CreateOrthographic(1.0f, 1.0f, 1.0f, 10.0f);
	m_ProjTM._11 = 1.0f;
	m_ProjTM._22 = 1.0f;

	m_pEnvironmentMap = std::make_shared<CubeMapRenderTarget>();
	BuildCubeMap(pDevice, 1024, 1024, 6, 1, m_pEnvironmentMap);

	m_pIrrdianceMap = std::make_shared<CubeMapRenderTarget>();
	BuildCubeMap(pDevice, 32, 32, 6, 1, m_pIrrdianceMap);

	m_pPreFilterMap = std::make_shared<CubeMapRenderTarget>();
	m_pPreFilterMap->Initialize(pDevice, PRE_FILTER_SIZE, PRE_FILTER_SIZE, 5);
}

void ReflectionProbe::SetTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pEnviormentTexture)
{
	m_pEnviormentTexture = pEnviormentTexture;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& ReflectionProbe::GetTextrue()
{
	return m_pEnviormentTexture;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ReflectionProbe::GetEnvironmentMap()
{
	return m_pEnvironmentMap->GetShaderResourceViewRawptr();
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ReflectionProbe::GetIrradianceMap()
{
	return m_pIrrdianceMap->GetShaderResourceViewRawptr();
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ReflectionProbe::GetPreFilterMap()
{
	return m_pPreFilterMap->GetShaderResourceViewRawptr();
}


Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& ReflectionProbe::GetEnvironmentTexture()
{
	return m_pEnviormentTexture;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& ReflectionProbe::GetIrradianceTexture()
{
	return m_pIrradianceTexture;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& ReflectionProbe::GetPreFilterTexture()
{
	return m_pPreFilterTexture;
}

void ReflectionProbe::SetEnvironmentMap(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTexture)
{
	m_pEnviormentTexture = pTexture;
}

void ReflectionProbe::SetIrradianceMap(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTexture)
{
	m_pIrradianceTexture = pTexture;
}

void ReflectionProbe::SetPreFilterMap(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTexture)
{
	m_pPreFilterTexture = pTexture;
}

std::shared_ptr<CubeMapRenderTarget>& ReflectionProbe::GetEnviormentCubeRenderTarget()
{
	return m_pEnvironmentMap;
}

std::shared_ptr<CubeMapRenderTarget>& ReflectionProbe::GetIrrdaianceCubeRenderTarget()
{
	return m_pIrrdianceMap;
}

std::shared_ptr<CubeMapRenderTarget>& ReflectionProbe::GetPreFilterCubeRenderTarget()
{
	return m_pPreFilterMap;
}


std::shared_ptr<ProbeHelper> ReflectionProbe::GetCube()
{
	return m_Cube;
}

std::shared_ptr<ProbeHelper> ReflectionProbe::GetSphere()
{
	return m_Sphere;
}

void ReflectionProbe::SetBakedMapIndex(
	unsigned int envMap, unsigned int irrMap, unsigned int filterMap)
{
	m_BakedMapIndex[0] = envMap; m_BakedMapIndex[1] = irrMap; m_BakedMapIndex[2] = filterMap;
}

const DirectX::SimpleMath::Matrix& ReflectionProbe::GetViewTM(unsigned int idx)
{
	return m_ViewTM_Arr[idx];
}

const DirectX::SimpleMath::Matrix& ReflectionProbe::GetProjTM()
{
	return m_ProjTM;
}

const DirectX::SimpleMath::Matrix& ReflectionProbe::GetProbeWorldTM()
{
	return m_ProbeWorldTM;
}

void ReflectionProbe::SetViewTM(unsigned int idx, const DirectX::SimpleMath::Matrix& viewTM)
{
	m_ViewTM_Arr[idx] = viewTM;
}

void ReflectionProbe::SetProjTM(const DirectX::SimpleMath::Matrix& projTM)
{
	m_ProjTM = projTM;
}

void ReflectionProbe::SetProbeWorldTM(const DirectX::SimpleMath::Matrix& worldTM)
{
	m_ProbeWorldTM = worldTM;
}

unsigned int ReflectionProbe::GetIrradianceTextureIndex()
{
	return m_BakedMapIndex[1];
}

unsigned int ReflectionProbe::GetPreFilterTextureIndex()
{
	return m_BakedMapIndex[2];
}

void ReflectionProbe::BuildHelper(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	std::vector<Vertex::Vertex2D> _cubeVertices;
	_cubeVertices.resize(24);

	const float length = 2.0f;
	const float width = 2.0f;
	const float height = 2.0f;
	XMFLOAT3 p0 = XMFLOAT3(-length * .5f, -width * .5f, height * .5f);
	XMFLOAT3 p1 = XMFLOAT3(length * .5f, -width * .5f, height * .5f);
	XMFLOAT3 p2 = XMFLOAT3(length * .5f, -width * .5f, -height * .5f);
	XMFLOAT3 p3 = XMFLOAT3(-length * .5f, -width * .5f, -height * .5f);

	XMFLOAT3 p4 = XMFLOAT3(-length * .5f, width * .5f, height * .5f);
	XMFLOAT3 p5 = XMFLOAT3(length * .5f, width * .5f, height * .5f);
	XMFLOAT3 p6 = XMFLOAT3(length * .5f, width * .5f, -height * .5f);
	XMFLOAT3 p7 = XMFLOAT3(-length * .5f, width * .5f, -height * .5f);

	_cubeVertices[0].pos = p0;
	_cubeVertices[1].pos = p1;
	_cubeVertices[2].pos = p2;
	_cubeVertices[3].pos = p3;

	_cubeVertices[4].pos = p7;
	_cubeVertices[5].pos = p4;
	_cubeVertices[6].pos = p0;
	_cubeVertices[7].pos = p3;

	_cubeVertices[8].pos = p4;
	_cubeVertices[9].pos = p5;
	_cubeVertices[10].pos = p1;
	_cubeVertices[11].pos = p0;
	_cubeVertices[12].pos = p6;
	_cubeVertices[13].pos = p7;
	_cubeVertices[14].pos = p3;
	_cubeVertices[15].pos = p2;
	_cubeVertices[16].pos = p5;
	_cubeVertices[17].pos = p6;
	_cubeVertices[18].pos = p2;
	_cubeVertices[19].pos = p1;
	_cubeVertices[20].pos = p7;
	_cubeVertices[21].pos = p6;
	_cubeVertices[22].pos = p5;
	_cubeVertices[23].pos = p4;

	XMFLOAT2 _00 = XMFLOAT2(0.0f, 0.0f);
	XMFLOAT2 _10 = XMFLOAT2(1.0f, 0.0f);
	XMFLOAT2 _01 = XMFLOAT2(0.0f, 1.0f);
	XMFLOAT2 _11 = XMFLOAT2(1.0f, 1.0f);

	for (int i = 0; i < 6; ++i)
	{
		_cubeVertices[i * 4].texCoord = _00;
		_cubeVertices[i * 4 + 1].texCoord = _10;
		_cubeVertices[i * 4 + 2].texCoord = _11;
		_cubeVertices[i * 4 + 3].texCoord = _01;
	}

	std::vector<UINT> _cubeIndices;
	_cubeIndices.resize(36);
	int index = 0;
	for (int i = 0; i < 6; ++i)
	{
		_cubeIndices[index++] = i * 4;
		_cubeIndices[index++] = i * 4 + 1;
		_cubeIndices[index++] = i * 4 + 3;
		_cubeIndices[index++] = i * 4 + 1;
		_cubeIndices[index++] = i * 4 + 2;
		_cubeIndices[index++] = i * 4 + 3;
	}

	{
		ID3D11Buffer* _vb;
		ID3D11Buffer* _ib;

		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::Vertex2D) * (UINT)_cubeVertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = _cubeVertices.data();
		pDevice->CreateBuffer(&vbd, &vinitData, &_vb);

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * (UINT)_cubeIndices.size();
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = _cubeIndices.data();
		pDevice->CreateBuffer(&ibd, &iinitData, &_ib);

		m_Cube = std::make_shared<ProbeHelper>();
		m_Cube->m_VertexBuffer.Swap(_vb);
		m_Cube->m_IndexBuffer.Swap(_ib);
		m_Cube->m_Stride = sizeof(Vertex::Vertex2D);
		m_Cube->m_IndexCount = (UINT)_cubeIndices.size();
	}


	float _radius = 50.0f;
	UINT _sliceCount = 30;
	UINT _statckCount = 30;

	std::vector<Vertex::Vertex> _vertices;
	std::vector<UINT> _indices;

	Vertex::Vertex _topVertex(0.0f, +_radius, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex::Vertex _bottomVertex(0.0f, -_radius, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);

	_vertices.push_back(_topVertex);


	float phiStep = XM_PI / _statckCount;
	float thetaStep = 2.0f * XM_PI / _sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= _statckCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (UINT j = 0; j <= _sliceCount; ++j)
		{
			float theta = j * thetaStep;

			Vertex::Vertex v;

			// spherical to cartesian
			v.pos.x = _radius * sinf(phi) * cosf(theta);
			v.pos.y = _radius * cosf(phi);
			v.pos.z = _radius * sinf(phi) * sinf(theta);

			XMVECTOR p = XMLoadFloat3(&v.pos);
			XMStoreFloat3(&v.normal, XMVector3Normalize(p));

			v.texCoord.x = theta / XM_2PI;
			v.texCoord.y = phi / XM_PI;

			_vertices.push_back(v);
		}
	}

	_vertices.push_back(_bottomVertex);
	for (UINT i = 1; i <= _sliceCount; ++i)
	{
		_indices.push_back(0);
		_indices.push_back(i + 1);
		_indices.push_back(i);
	}

	UINT baseIndex = 1;
	UINT ringVertexCount = _sliceCount + 1;
	for (UINT i = 0; i < _statckCount - 2; ++i)
	{
		for (UINT j = 0; j < _sliceCount; ++j)
		{
			_indices.push_back(baseIndex + i * ringVertexCount + j);
			_indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			_indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			_indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			_indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			_indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	UINT southPoleIndex = (UINT)_vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < _sliceCount; ++i)
	{
		_indices.push_back(southPoleIndex);
		_indices.push_back(baseIndex + i);
		_indices.push_back(baseIndex + i + 1);
	}

	{
		ID3D11Buffer* _vb;
		ID3D11Buffer* _ib;

		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex::Vertex) * (UINT)_vertices.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = _vertices.data();
		pDevice->CreateBuffer(&vbd, &vinitData, &_vb);

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT) * (UINT)_indices.size();
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = _indices.data();
		pDevice->CreateBuffer(&ibd, &iinitData, &_ib);

		m_Sphere = std::make_shared<ProbeHelper>();
		m_Sphere->m_VertexBuffer.Swap(_vb);
		m_Sphere->m_IndexBuffer.Swap(_ib);
		m_Sphere->m_Stride = sizeof(Vertex::Vertex);
		m_Sphere->m_IndexCount = (UINT)_indices.size();
	}
}

void ReflectionProbe::BuildCubeMap(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	UINT width, UINT height, UINT renderTargetCount, UINT mipLevel,
	std::shared_ptr<CubeMapRenderTarget>& pCubeMap)
{
	// info는 내가 만든 렌더타겟베이스 같은 느낌
	// 그냥 큐브맵 하나를 만든다고 생각하자

	DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	HRESULT _hr;
	{
		try
		{
			// 큐브의 한면을 담당하는 텍스쳐
			//	Create Texture2D
			D3D11_TEXTURE2D_DESC desc;
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = mipLevel;
			desc.ArraySize = renderTargetCount;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Format = format;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			desc.CPUAccessFlags = 0;
			if (renderTargetCount != 1)
				desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
			else desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

			_hr = pDevice->CreateTexture2D(&desc, NULL, pCubeMap->GetTexture2D().GetAddressOf());
			COM_ERROR_IF_FAILED(_hr, "Create Textrue2D Fail");


			//	Create RTVs(Render Target Views)
			D3D11_RENDER_TARGET_VIEW_DESC _rtvDesc;
			_rtvDesc.Format = format;
			if (renderTargetCount == 1)
				_rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			else
				_rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			_rtvDesc.Texture2DArray.ArraySize = 1;
			_rtvDesc.Texture2DArray.MipSlice = 0;

			for (UINT i = 0; i < renderTargetCount; i++)
			{
				_rtvDesc.Texture2DArray.FirstArraySlice = i;

				_hr = pDevice->CreateRenderTargetView(pCubeMap->GetTexture2D().Get(), &_rtvDesc,
					pCubeMap->GetRenderTargetView(i).GetAddressOf());

				COM_ERROR_IF_FAILED(_hr, "Create RenderTargetView Fail");

			}

			//	Create SRV(Shader Resource View)
			D3D11_SHADER_RESOURCE_VIEW_DESC _srvDesc;
			_srvDesc.Format = format;
			if (renderTargetCount == 1)
				_srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			else
				_srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			_srvDesc.TextureCube.MostDetailedMip = 0;
			_srvDesc.TextureCube.MipLevels = -1;

			_hr = pDevice->CreateShaderResourceView(pCubeMap->GetTexture2D().Get(), &_srvDesc,
				pCubeMap->GetShaderResourceViewRawptr().GetAddressOf());
			COM_ERROR_IF_FAILED(_hr, "Create ShaderResourceView Fail");


			{
				// 뎁스 스텐실 뷰 만드는 곳
				Microsoft::WRL::ComPtr<ID3D11Texture2D> _texture;
				D3D11_TEXTURE2D_DESC _2dDesc;
				_2dDesc.Width = width;
				_2dDesc.Height = height;
				_2dDesc.MipLevels = 1;
				_2dDesc.ArraySize = 1;
				_2dDesc.SampleDesc.Count = 1;
				_2dDesc.SampleDesc.Quality = 0;
				_2dDesc.Format = DXGI_FORMAT_D32_FLOAT;
				_2dDesc.Usage = D3D11_USAGE_DEFAULT;
				_2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				_2dDesc.CPUAccessFlags = 0;
				_2dDesc.MiscFlags = 0;

				_hr = pDevice->CreateTexture2D(&_2dDesc, NULL, _texture.GetAddressOf());
				COM_ERROR_IF_FAILED(_hr, "Create Textrue2D Fail");


				D3D11_DEPTH_STENCIL_VIEW_DESC _dsvDesc;
				_dsvDesc.Flags = 0;
				_dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
				_dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				_dsvDesc.Texture2D.MipSlice = 0;

				_hr = pDevice->CreateDepthStencilView(_texture.Get(), &_dsvDesc, pCubeMap->GetDepthStencilView().GetAddressOf());

				COM_ERROR_IF_FAILED(_hr, "Create DepthStencilView Fail");
			}
		}
		catch (COMException& e)
		{
			ErrorLogger::Log(e);
		}
	}

	// 뷰포트가 필요해서 만들어준다
	pCubeMap->CreateViewPort(width, height);
}



IBL::IBL()
{

}

IBL::~IBL()
{
	m_ReflectionProbe_V.clear();
	m_BaseReflectionProbe.reset();
}

void IBL::Initialize(Microsoft::WRL::ComPtr <ID3D11Device> pDevice)
{
	m_BaseReflectionProbe = std::make_unique<ReflectionProbe>();
	m_BaseReflectionProbe->Initialize(pDevice);
}

unsigned int IBL::AddReflectionProbe(Microsoft::WRL::ComPtr <ID3D11Device> pDevice)
{
	//std::shared_ptr<ReflectionProbe> _newReflectionProbe = std::make_shared<ReflectionProbe>();
	//_newReflectionProbe->Initialize(pDevice);
	//m_ReflectionProbe_V.push_back(_newReflectionProbe);
	m_ReflectionProbe_V.emplace_back(std::make_unique<ReflectionProbe>());
	m_ReflectionProbe_V[m_ReflectionProbe_V.size() - 1]->Initialize(pDevice);


	return static_cast<unsigned int>(m_ReflectionProbe_V.size() - 1);
}

bool IBL::IsProbeExist()
{
	if (m_ReflectionProbe_V.empty() == true) return false;
	else return true;
}

ReflectionProbe* IBL::GetReflectionProbe(unsigned int idx)
{
	return m_ReflectionProbe_V[idx].get();
}

ReflectionProbe* IBL::GetBasicIBL()
{
	return m_BaseReflectionProbe.get();
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> IBL::GetIrradianceMapSRV(unsigned int idx)
{
	if (m_ReflectionProbe_V.empty() == true || idx >= m_ReflectionProbe_V.size())
	{
		return Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>(nullptr);
	}
	else { return m_ReflectionProbe_V[idx]->GetIrradianceMap(); }
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> IBL::GetPrefilterMapSRV(unsigned int idx)
{
	if (m_ReflectionProbe_V.empty() == true || idx >= m_ReflectionProbe_V.size())
	{
		return Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>(nullptr);
	}
	else { return m_ReflectionProbe_V[idx]->GetPreFilterMap(); }
}

void IBL::ResetReflectionProbe(unsigned int idx, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pEnviormentTexture)
{
	m_ReflectionProbe_V[idx]->m_BakingTimer = 0;
	m_ReflectionProbe_V[idx]->SetTexture(pEnviormentTexture);
}

void IBL::ReflectionProbeRenderTargetViewBind(
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext,
	const unsigned int probeIndex, const unsigned int faceIndex,
	ID3D11DepthStencilView* pDepth)
{
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _nowRTV;
	auto _nowReflectionProbe = m_ReflectionProbe_V[probeIndex].get();

	_nowRTV = _nowReflectionProbe->GetEnviormentCubeRenderTarget()->GetRenderTargetView(faceIndex);

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->RSSetState(RasterizerState::GetSolidNormal());
	pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 0);
	pDeviceContext->OMSetBlendState(RasterizerState::GetBlenderState(), NULL, 0xFFFFFFFF);
	pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());


	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _nowDSV = _nowReflectionProbe->GetEnviormentCubeRenderTarget()->GetDepthStencilView();

	pDeviceContext->OMSetRenderTargets(1, _nowRTV.GetAddressOf(), _nowDSV.Get());
	pDeviceContext->ClearRenderTargetView(_nowRTV.Get(), ClearColor);
	pDeviceContext->ClearDepthStencilView(_nowDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
	pDeviceContext->RSSetViewports(1, &_nowReflectionProbe->GetEnviormentCubeRenderTarget()->GetViewPort());
}

void IBL::BakeReflectionProbe(
	Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext,
	std::shared_ptr<VertexShader> pVertexShader,
	std::shared_ptr<PixelShader> pIrradianceShader,
	std::shared_ptr<PixelShader> pBakePixelShader,
	unsigned int sceneIndex,
	const std::string& sceneName,
	unsigned int reflectionProbeIndex)
{

	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// 이 단계에서는 이미 ReflectionProbeRender에서 그림을 그린 뒤라는 전제
	// 따라서 IrradianceMap, PreFilterMap을 만들면 된다 IBL에서 필요한게 그 두개니까
	//std::shared_ptr<ReflectionProbe> _nowReflectionProbe = m_ReflectionProbe_V[reflectionProbeIndex];
	auto _nowReflectionProbe = m_ReflectionProbe_V[reflectionProbeIndex].get();

	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _environment = _nowReflectionProbe->GetEnvironmentMap();

	UINT offset = 0;
	UINT stride = _nowReflectionProbe->GetCube()->m_Stride;

	Microsoft::WRL::ComPtr<ID3D11Buffer> _vertexBuffer = _nowReflectionProbe->GetCube()->m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _indexBuffer = _nowReflectionProbe->GetCube()->m_IndexBuffer;
	unsigned int _indexCount = _nowReflectionProbe->GetCube()->m_IndexCount;

	pDeviceContext->RSSetState(RasterizerState::GetSolidRS());

	// Irradiance Map 패스
	std::shared_ptr<CubeMapRenderTarget> _irradianceCubeMap = _nowReflectionProbe->GetIrrdaianceCubeRenderTarget();
	for (int i = 0; i < 6; i++)
	{
		// 큐브맵으로 이루어진 렌더 타겟으로부터 하나씩 렌더 타겟을 가져와 초기화한다
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _nowRTV = _irradianceCubeMap->GetRenderTargetView(i);
		pDeviceContext->OMSetRenderTargets(1, _nowRTV.GetAddressOf(), _irradianceCubeMap->GetDepthStencilView().Get());
		pDeviceContext->ClearRenderTargetView(_nowRTV.Get(), ClearColor);
		pDeviceContext->ClearDepthStencilView(_irradianceCubeMap->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
		pDeviceContext->RSSetViewports(1, &_irradianceCubeMap->GetViewPort());

		// 사용할 셰이더들을 묶어준다, IrradianceMap(조도맵)을 만들어주는 픽셀 셰이더를 셋 해준다
		pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
		pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
		pDeviceContext->PSSetShader(pIrradianceShader->GetPixelShader(), NULL, 0);
		pDeviceContext->PSSetShaderResources(0, 1, _nowReflectionProbe->GetEnvironmentMap().GetAddressOf());

		// 상수 버퍼에 현재 Reflection Probe의 6방향에 대한 ViewProjection 정보를 넣어준다
		ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix = _nowReflectionProbe->GetViewTM(i) * _nowReflectionProbe->GetProjTM();
		ConstantBufferManager::GetWVPMatrix()->ApplyChanges();
		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetWVPMatrix()->GetAddressOf());
		pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

		pDeviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->DrawIndexed(_indexCount, 0, 0);
	}

	// PreFilter Map 패스
	std::vector<std::shared_ptr<RenderTargetView>> _cubeFaces;
	_cubeFaces.resize(6);
	for (int mip = 0; mip < 5; ++mip)
	{
		// 밉맵의 크기 계산 및 이에 따른 렌더 타겟 초기화
		const unsigned int mipWidth = unsigned int(PRE_FILTER_SIZE * pow(0.5, mip));
		const unsigned int mipHeight = unsigned int(PRE_FILTER_SIZE * pow(0.5, mip));

		for (int i = 0; i < 6; ++i)
		{
			std::shared_ptr<RenderTargetView> _newRTV = std::make_shared<RenderTargetView>();
			_newRTV->Initialize(pDevice, mipWidth, mipHeight, 1);
			_cubeFaces[i] = _newRTV;
		}

		// PreFilter맵은 밉맵들이 거칠기에 영향을 받으므로 상수 버퍼를 넘겨준다
		const float roughness = float(mip) / 4.0f;
		ConstantBufferManager::GetPSRoughnessBuffer()->data.Roughness = roughness;
		ConstantBufferManager::GetPSRoughnessBuffer()->ApplyChanges();

		for (int i = 0; i < 6; ++i)
		{
			// 위에서 초기화한 렌더 타겟을 가져와서 PreFilter Map을 렌더링한다
			std::shared_ptr<RenderTargetView> _nowRTV = _cubeFaces[i];

			pDeviceContext->OMSetRenderTargets(1, _nowRTV->GetRenderTargetViewAddressOf(), nullptr);
			pDeviceContext->ClearRenderTargetView(_nowRTV->GetRenderTargerViewRawptr(), ClearColor);
			pDeviceContext->RSSetViewports(1, &_nowRTV->GetViewPort());

			pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
			pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
			pDeviceContext->PSSetShader(pBakePixelShader->GetPixelShader(), NULL, 0);
			pDeviceContext->PSSetShaderResources(0, 1, _nowReflectionProbe->GetEnvironmentMap().GetAddressOf());

			// 조도맵과 마찬가지로 6방향에 대한 ViewProjection 정보를 넣는다
			ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix = _nowReflectionProbe->GetViewTM(i) * _nowReflectionProbe->GetProjTM();
			ConstantBufferManager::GetWVPMatrix()->ApplyChanges();

			pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetWVPMatrix()->GetAddressOf());

			// for문 밖에서 정의한 거칠기를 넘겨준다
			pDeviceContext->PSSetConstantBuffers(0, 1, ConstantBufferManager::GetPSRoughnessBuffer()->GetAddressOf());
			pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

			pDeviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
			pDeviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
			pDeviceContext->DrawIndexed(_indexCount, 0, 0);
		}

		// 6방향의 렌더 타겟에 렌더링이 끝났다면 이 vector를 넘겨서 단계별 밉맵을 생성한다
		_nowReflectionProbe->GetPreFilterCubeRenderTarget()->CreateMipMap(pDeviceContext, _cubeFaces, mipWidth, mipHeight, mip);
	}


	{
		// 각 텍스쳐 성분을 구분하기 위한 네이밍 작업
		//std::string _environmentMapName(std::to_string(sceneIndex));
		//std::string _irradianceMapName(std::to_string(sceneIndex));
		//std::string _prefilterMapName(std::to_string(sceneIndex));

		std::string _environmentMapName(sceneName);
		std::string _irradianceMapName(sceneName);
		std::string _prefilterMapName(sceneName);

		_environmentMapName += ENV_MAP;
		_irradianceMapName += IRR_MAP;
		_prefilterMapName += FILTER_MAP;

		// .ibl 자원을 관리하기 위해 확장자 명을 수정
		_environmentMapName += std::to_string(reflectionProbeIndex) + ".ibl";
		_irradianceMapName += std::to_string(reflectionProbeIndex) + ".ibl";
		_prefilterMapName += std::to_string(reflectionProbeIndex) + ".ibl";

		_environmentMapName = ENV_PATH + _environmentMapName;
		_irradianceMapName = IRR_PATH + _irradianceMapName;
		_prefilterMapName = FILTER_PATH + _prefilterMapName;

		// DirectXTex 라이브러리에서 DDS 포맷으로 텍스쳐를 저장하는 함수를 만들어 호출
		TextureCapture::SaveTextureToDDSFile(pDevice, pDeviceContext, 
			_nowReflectionProbe->GetEnviormentCubeRenderTarget()->GetTexture2D(), StringHelper::StringToWchar(_environmentMapName));

		TextureCapture::SaveTextureToDDSFile(pDevice, pDeviceContext, 
			_nowReflectionProbe->GetIrrdaianceCubeRenderTarget()->GetTexture2D(), StringHelper::StringToWchar(_irradianceMapName));

		TextureCapture::SaveTextureToDDSFile(pDevice, pDeviceContext, 
			_nowReflectionProbe->GetPreFilterCubeRenderTarget()->GetTexture2D(), StringHelper::StringToWchar(_prefilterMapName));
	}


	_cubeFaces.clear();
}

bool IBL::BasicIrradiancePreFilterMap(
	Microsoft::WRL::ComPtr <ID3D11Device> pDevice,
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pEnviormentTexture,
	class VertexShader* pVertexShader,
	class PixelShader* pEnvironmentShader,
	class PixelShader* pIrradianceShader,
	class PixelShader* pBakePixelShader,
	unsigned int reflectionProbeIndex)
{

	//m_BaseReflectionProbe->m_BakingTimer++;
	//
	//if (m_BaseReflectionProbe->m_BakingTimer > 2)
	//{
	//	return true;
	//}


	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pTextrue;
	if (pEnviormentTexture != nullptr) { _pTextrue = pEnviormentTexture; }
	else _pTextrue = m_BaseReflectionProbe->GetTextrue();


	// 베이킹 하는데 2프레임에 걸쳐서 진행됨
	// 베이킹이 오래걸렸던 이유는 큐브맵의 크기가 컸던 것(1024였음)
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->RSSetState(RasterizerState::GetNoCullingRS());
	pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 0);
	pDeviceContext->OMSetBlendState(RasterizerState::GetBlenderState(), NULL, 0xFFFFFFFF);
	pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());


	float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };


	std::shared_ptr<ProbeHelper> _cube = m_BaseReflectionProbe->GetCube();
	std::shared_ptr<CubeMapRenderTarget> _EnvironmentCubeMap = m_BaseReflectionProbe->GetEnviormentCubeRenderTarget();
	// 매핑 패스
	for (int i = 0; i < 6; i++)
	{
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _nowRTV = _EnvironmentCubeMap->GetRenderTargetView(i);

		pDeviceContext->OMSetRenderTargets(1, _nowRTV.GetAddressOf(), _EnvironmentCubeMap->GetDepthStencilView().Get());
		pDeviceContext->ClearRenderTargetView(_nowRTV.Get(), ClearColor);
		pDeviceContext->ClearDepthStencilView(_EnvironmentCubeMap->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
		pDeviceContext->RSSetViewports(1, &_EnvironmentCubeMap->GetViewPort());

		pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
		pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
		pDeviceContext->PSSetShader(pEnvironmentShader->GetPixelShader(), NULL, 0);

		ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix = m_BaseReflectionProbe->GetViewTM(i) * m_BaseReflectionProbe->GetProjTM();
		ConstantBufferManager::GetWVPMatrix()->ApplyChanges();

		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetWVPMatrix()->GetAddressOf());
		pDeviceContext->PSSetShaderResources(0, 1, pEnviormentTexture.GetAddressOf());

		pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

		UINT offset = 0;
		UINT stride = _cube->m_Stride;

		pDeviceContext->IASetVertexBuffers(0, 1, _cube->m_VertexBuffer.GetAddressOf(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(_cube->m_IndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->DrawIndexed(_cube->m_IndexCount, 0, 0);
	}

	// Irradiance Map 패스
	std::shared_ptr<CubeMapRenderTarget> _irradianceCubeMap = m_BaseReflectionProbe->GetIrrdaianceCubeRenderTarget();
	for (int i = 0; i < 6; i++)
	{
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _nowRTV = _irradianceCubeMap->GetRenderTargetView(i);

		pDeviceContext->OMSetRenderTargets(1, _nowRTV.GetAddressOf(), _irradianceCubeMap->GetDepthStencilView().Get());
		pDeviceContext->ClearRenderTargetView(_nowRTV.Get(), ClearColor);
		pDeviceContext->ClearDepthStencilView(_irradianceCubeMap->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
		pDeviceContext->RSSetViewports(1, &_irradianceCubeMap->GetViewPort());

		pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
		pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
		pDeviceContext->PSSetShader(pIrradianceShader->GetPixelShader(), NULL, 0);

		ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix = m_BaseReflectionProbe->GetViewTM(i) * m_BaseReflectionProbe->GetProjTM();
		ConstantBufferManager::GetWVPMatrix()->ApplyChanges();

		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetWVPMatrix()->GetAddressOf());
		pDeviceContext->PSSetShaderResources(0, 1, pEnviormentTexture.GetAddressOf());
		pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

		UINT offset = 0;
		UINT stride = _cube->m_Stride;

		pDeviceContext->IASetVertexBuffers(0, 1, _cube->m_VertexBuffer.GetAddressOf(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(_cube->m_IndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->DrawIndexed(_cube->m_IndexCount, 0, 0);
	}


	// PreFilter Map 패스
	std::vector<std::shared_ptr<RenderTargetView>> _cubeFaces;
	_cubeFaces.resize(6);
	for (int mip = 0; mip < 5; ++mip)
	{
		const unsigned int mipWidth = unsigned int(PRE_FILTER_SIZE * pow(0.5, mip));
		const unsigned int mipHeight = unsigned int(PRE_FILTER_SIZE * pow(0.5, mip));

		for (int i = 0; i < 6; ++i)
		{
			std::shared_ptr<RenderTargetView> _newRTV = std::make_shared<RenderTargetView>();
			_newRTV->Initialize(pDevice, mipWidth, mipHeight, 1);
			_cubeFaces[i] = _newRTV;
		}

		const float roughness = float(mip) / 4.0f;
		ConstantBufferManager::GetPSRoughnessBuffer()->data.Roughness = roughness;
		ConstantBufferManager::GetPSRoughnessBuffer()->ApplyChanges();

		for (int i = 0; i < 6; ++i)
		{
			std::shared_ptr<RenderTargetView> _nowRTV = _cubeFaces[i];

			pDeviceContext->OMSetRenderTargets(1, _nowRTV->GetRenderTargetViewAddressOf(), nullptr);
			pDeviceContext->ClearRenderTargetView(_nowRTV->GetRenderTargerViewRawptr(), ClearColor);
			pDeviceContext->RSSetViewports(1, &_nowRTV->GetViewPort());

			pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
			pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
			pDeviceContext->PSSetShader(pBakePixelShader->GetPixelShader(), NULL, 0);

			ConstantBufferManager::GetWVPMatrix()->data.wvpMatrix = m_BaseReflectionProbe->GetViewTM(i) * m_BaseReflectionProbe->GetProjTM();
			ConstantBufferManager::GetWVPMatrix()->ApplyChanges();

			pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetWVPMatrix()->GetAddressOf());
			pDeviceContext->PSSetShaderResources(0, 1, m_BaseReflectionProbe->GetEnviormentCubeRenderTarget()->GetShaderResourceViewAddressOf());
			pDeviceContext->PSSetConstantBuffers(0, 1, ConstantBufferManager::GetPSRoughnessBuffer()->GetAddressOf());

			pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

			UINT offset = 0;
			UINT stride = _cube->m_Stride;

			pDeviceContext->IASetVertexBuffers(0, 1, _cube->m_VertexBuffer.GetAddressOf(), &stride, &offset);
			pDeviceContext->IASetIndexBuffer(_cube->m_IndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
			pDeviceContext->DrawIndexed(_cube->m_IndexCount, 0, 0);
		}

		m_BaseReflectionProbe->GetPreFilterCubeRenderTarget()->CreateMipMap(pDeviceContext, _cubeFaces, mipWidth, mipHeight, mip);
	}


	{
		std::string _environmentMapName("basic");
		std::string _irradianceMapName("basic");
		std::string _prefilterMapName("basic");

		_environmentMapName += ENV_MAP;
		_irradianceMapName += IRR_MAP;
		_prefilterMapName += FILTER_MAP;

		// .ibl 자원을 관리하기 위해 확장자 명을 수정
		_environmentMapName += std::to_string(reflectionProbeIndex) + ".ibl";
		_irradianceMapName += std::to_string(reflectionProbeIndex) + ".ibl";
		_prefilterMapName += std::to_string(reflectionProbeIndex) + ".ibl";

		_environmentMapName = ENV_PATH + _environmentMapName;
		_irradianceMapName = IRR_PATH + _irradianceMapName;
		_prefilterMapName = FILTER_PATH + _prefilterMapName;

		// DirectXTex 라이브러리에서 DDS 포맷으로 텍스쳐를 저장하는 함수를 만들어 호출
		TextureCapture::SaveTextureToDDSFile(pDevice, pDeviceContext,
			m_BaseReflectionProbe->GetEnviormentCubeRenderTarget()->GetTexture2D(), StringHelper::StringToWchar(_environmentMapName));

		TextureCapture::SaveTextureToDDSFile(pDevice, pDeviceContext,
			m_BaseReflectionProbe->GetIrrdaianceCubeRenderTarget()->GetTexture2D(), StringHelper::StringToWchar(_irradianceMapName));

		TextureCapture::SaveTextureToDDSFile(pDevice, pDeviceContext,
			m_BaseReflectionProbe->GetPreFilterCubeRenderTarget()->GetTexture2D(), StringHelper::StringToWchar(_prefilterMapName));
	}

	_cubeFaces.clear();


	return true;
}

void IBL::RenderProbe(
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext,
	class VertexShader* pVertexShader,
	class PixelShader* pPixelShader,
	const DirectX::SimpleMath::Matrix& viewTM,
	const DirectX::SimpleMath::Matrix& projTM)
{
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (unsigned int i = 0; i < m_ReflectionProbe_V.size(); i++)
	{
		std::shared_ptr<ProbeHelper> _sphere = m_ReflectionProbe_V[i]->GetSphere();

		pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
		pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
		pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);

		ConstantBufferManager::GetVertexBuffer()->data.worldMatrix =
			DirectX::SimpleMath::Matrix::CreateScale(0.005f) * m_ReflectionProbe_V[i]->GetProbeWorldTM();
		ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix =
			DirectX::SimpleMath::Matrix::CreateScale(0.005f) * m_ReflectionProbe_V[i]->GetProbeWorldTM() * viewTM * projTM;
		ConstantBufferManager::GetVertexBuffer()->ApplyChanges();

		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer()->GetAddressOf());
		pDeviceContext->PSSetShaderResources(0, 1, m_ReflectionProbe_V[i]->GetEnviormentCubeRenderTarget()->GetShaderResourceViewAddressOf());
		pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

		UINT stride = _sphere->m_Stride;
		UINT offset = 0;

		pDeviceContext->IASetVertexBuffers(0, 1, _sphere->m_VertexBuffer.GetAddressOf(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(_sphere->m_IndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->DrawIndexed(_sphere->m_IndexCount, 0, 0);
	}
}

void IBL::ClearReflectionProbe()
{
	m_ReflectionProbe_V.clear();
}
