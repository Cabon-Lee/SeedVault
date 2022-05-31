#include "LightClass.h"

#include "DepthStencilView.h"
#include "RenderTargetView.h"
#include "RasterizerState.h"

#include "ConstantBufferManager.h"
#include "SimpleMath.h"

LightClass::LightClass()
	: m_LightIndex(-1)
	, m_LightType(0)
	, m_ShadowMapDepthStencil(nullptr)
	, m_ShadowMapRenderTarget(nullptr)
	, m_LightWorldTM()
	, m_ZeroTM()
	, m_ViewTM()
	, m_ProjTM()
	, m_ViewProjTM()
	, m_ShadowTM()
	, m_DirectionalRadius(0.0f)
	, m_Center()
	, m_Direction()
	, m_LightPos()
	, m_YawPitchRoll()
	, m_IsTextured(false)
	, m_TextureIndex(0)
{
	m_Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_DirectionalRadius = sqrtf(10.0f * 10.0f + 15.0f * 15.0f);
	m_ZeroTM = XMMatrixIdentity();

	m_ZeroTM._11 = 0.0f;
	m_ZeroTM._22 = 0.0f;
	m_ZeroTM._33 = 0.0f;
	m_ZeroTM._44 = 0.0f;

}

LightClass::~LightClass()
{

}

void LightClass::BuildShadowMap(ID3D11Device* pDevice)
{
	m_ShadowMapDepthStencil = std::make_shared<DepthStencilView>();
	m_ShadowMapDepthStencil->Initialize(pDevice, SHDOWMAP_SIZE, SHDOWMAP_SIZE,
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R24G8_TYPELESS,
		DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
		DXGI_FORMAT_D24_UNORM_S8_UINT);

	m_ShadowMapRenderTarget = std::make_shared<RenderTargetView>();
	m_ShadowMapRenderTarget->Initialize(pDevice, SHDOWMAP_SIZE, SHDOWMAP_SIZE,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32A32_FLOAT);

}

void LightClass::ClearShadowRenderTarget(ID3D11DeviceContext* deviceContext)
{
	float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	deviceContext->ClearRenderTargetView(m_ShadowMapRenderTarget->GetRenderTargerViewRawptr(), ClearColor);
}

void LightClass::BindShadowMap(ID3D11DeviceContext* deviceContext)
{
	deviceContext->RSSetViewports(1, &m_ShadowMapDepthStencil->GetViewPort());
	ID3D11RenderTargetView* _renderTargets[1] = { nullptr };

	//---------------------------------------------------------------------------------------
	// 셰이더에 묶여있는 자원을 풀어준다
	// 뎁스-스텐실 자원을 초기화한다
	//---------------------------------------------------------------------------------------
	deviceContext->OMSetRenderTargets(1, _renderTargets, m_ShadowMapDepthStencil->GetDepthSetncilView());

	deviceContext->ClearDepthStencilView(
		m_ShadowMapDepthStencil->GetDepthSetncilView(), D3D11_CLEAR_DEPTH, 1.0, 0);
	deviceContext->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 1);
}

void LightClass::BindTexture(ID3D11DeviceContext* deviceContext)
{
	// 실험 중
	// 뎁스 스텐실 버퍼를 끄고 렌더 타겟만 묶어서 그려본다
	deviceContext->RSSetViewports(1, &m_ShadowMapDepthStencil->GetViewPort());
	
	ID3D11RenderTargetView* _renderTargets[1] = { m_ShadowMapRenderTarget->GetRenderTargerViewRawptr() };
	deviceContext->OMSetRenderTargets(1, _renderTargets, nullptr);

	float nullColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	deviceContext->ClearRenderTargetView(m_ShadowMapRenderTarget->GetRenderTargerViewRawptr(), nullColor);
}

void LightClass::LightUpdate(
	unsigned int lightIndex, unsigned int type, 
	const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir, const DirectX::XMFLOAT3& color, 
	float intensity, float falloffStart /*= 0.0f*/, float falloffEnd /*= 0.0f*/, float spotPower /*= 0.0f*/, 
	bool castShadow /*= false*/, bool isTextured /*= false*/, unsigned int textureIndex /*= 0*/)
{
	m_LightType = type;	// 디렉셔널, 포인트, 스팟

	m_IsTextured = isTextured;		// 라이트 텍스쳐를 쓰는가?
	m_TextureIndex = textureIndex;	// 쓴다면 텍스쳐 인덱스는?

	m_LightPos = pos;				// 라이트의 위치

	// rgb값에 intensity를 적용하여 범위를 늘렸다
	DirectX::SimpleMath::Vector3 _nowColor = color;
	_nowColor *= intensity * 10.f;

	// 현재 들어오고 있는 dir값은 YawPitchRoll이다
	m_YawPitchRoll = dir;
	m_Direction = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(dir.y, dir.x, dir.z).Forward();

	ConstantBufferManager::GetPSDirLightBuffer()->data.lights[lightIndex].Position = pos;
	ConstantBufferManager::GetPSDirLightBuffer()->data.lights[lightIndex].Direction = m_Direction;
	ConstantBufferManager::GetPSDirLightBuffer()->data.lights[lightIndex].Strength = _nowColor;
	ConstantBufferManager::GetPSDirLightBuffer()->data.lights[lightIndex].FalloffStart = falloffStart;
	ConstantBufferManager::GetPSDirLightBuffer()->data.lights[lightIndex].FalloffEnd = falloffEnd;
	ConstantBufferManager::GetPSDirLightBuffer()->data.lights[lightIndex].SpotPower = spotPower;
	ConstantBufferManager::GetPSDirLightBuffer()->data.lights[lightIndex].shadowInfo.x = type;

	// 64개의 배열에 현재 인덱스에 맞는 배열을 찾고, 거기에 맞는 type을 넣어준다
	int _nowArray = lightIndex / 4;
	int _elem = lightIndex % 4;

	switch (_elem)
	{
	case 0: ConstantBufferManager::GetLightCountBuffer()->data.lightType[_nowArray].x = type; break;
	case 1: ConstantBufferManager::GetLightCountBuffer()->data.lightType[_nowArray].y = type; break;
	case 2: ConstantBufferManager::GetLightCountBuffer()->data.lightType[_nowArray].z = type; break;
	case 3: ConstantBufferManager::GetLightCountBuffer()->data.lightType[_nowArray].w = type; break;
	}

	ConstantBufferManager::GetLightCountBuffer()->data.lightCount = lightIndex + 1;

	GetViewProjTM(type, pos, m_Direction);

	if (castShadow == true)
	{
		ConstantBufferManager::GetPSDirLightBuffer()->data.lights[lightIndex].shadowInfo.y = 1;
		ConstantBufferManager::GetShadowTM()->data.shadowTM[lightIndex] = m_ShadowTM;
	}
	else
	{
		ConstantBufferManager::GetPSDirLightBuffer()->data.lights[lightIndex].shadowInfo.y = 0;
		ConstantBufferManager::GetShadowTM()->data.shadowTM[lightIndex] = m_ZeroTM;
	}
}

const DirectX::XMMATRIX& LightClass::GetViewTM()
{
	return m_ViewTM;
}

const DirectX::XMMATRIX& LightClass::GetProjTM()
{
	return m_ProjTM;
}

void LightClass::GetViewProjTM(unsigned int type, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir)
{
	// pos, dir 둘다 zero vector라면 문제가 발생한다
	DirectX::SimpleMath::Vector3 _nowDir(dir);
	if (_nowDir.Length() == 0.0f)
	{
		_nowDir.x = 0.1f;
	}

	switch (type)
	{
	case DIRECTONAL_LIGHT:
		DirectionalLightViewProjTM(dir);
		break;
	case SPOT_LIGHT:
		SpotLightViewProjTM(pos, dir);
		break;
	}
}

DirectX::XMMATRIX& LightClass::GetViewProjTM()
{
	return m_ViewProjTM;
}

DirectX::XMMATRIX& LightClass::GetShadowTM()
{
	return m_ShadowTM;
}

const DirectX::XMFLOAT3& LightClass::GetLightPos()
{
	return m_LightPos;
}

DirectX::XMFLOAT3& LightClass::GetYawPitchRoll()
{
	return m_YawPitchRoll;
}

const DirectX::SimpleMath::Matrix& LightClass::GetLightWorldTM()
{
	// 디버깅용, 디버깅 안하면 빼자 연산 부하가 있다
	m_LightWorldTM =
		DirectX::SimpleMath::Matrix::CreateScale(1.0f) *
		DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(m_YawPitchRoll.y, m_YawPitchRoll.x, 0.0f) *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_LightPos);

	return m_LightWorldTM;
}

unsigned int LightClass::GetTextureIndex()
{
	return m_TextureIndex;
}

bool LightClass::GetIsTextured()
{
	return m_IsTextured;
}

void LightClass::DirectionalLightViewProjTM(const DirectX::XMFLOAT3& dir)
{
	XMVECTOR _lightDir = XMLoadFloat3(&dir);
	XMVECTOR m_LightPos = -2.0f * m_DirectionalRadius * _lightDir;
	XMVECTOR _targetPos = XMLoadFloat3(&m_Center);
	XMVECTOR _up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_ViewTM = XMMatrixLookAtLH(m_LightPos, _targetPos, _up);

	// 경계구를 광원 공간으로 변환한다
	XMFLOAT3 _sphereCenterLS;
	XMStoreFloat3(&_sphereCenterLS, XMVector3TransformCoord(_targetPos, m_ViewTM));

	// 장면을 감싸는 광원 공간 직교 투영 상자
	float _viewLeft = _sphereCenterLS.x - m_DirectionalRadius;
	float _viewBottom = _sphereCenterLS.y - m_DirectionalRadius;
	float _viewNear = _sphereCenterLS.z - m_DirectionalRadius;
	float _viewRight = _sphereCenterLS.x + m_DirectionalRadius;
	float _viewTop = _sphereCenterLS.y + m_DirectionalRadius;
	float _viewFar = _sphereCenterLS.z + m_DirectionalRadius;

	m_ProjTM = XMMatrixOrthographicOffCenterLH(_viewLeft, _viewRight, _viewBottom, _viewTop, _viewNear, _viewFar);

	// NDC 공간 [-1, +1]^2을 텍스처 공간 [0,1]^2으로 변환한다

	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	m_ShadowTM = m_ViewTM * m_ProjTM * T;

	m_ViewProjTM = XMMatrixMultiply(m_ViewTM, m_ProjTM);
}

void LightClass::SpotLightViewProjTM(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3 dir)
{
	// 위쪽을 가리키는 벡터를 설정합니다.
	DirectX::SimpleMath::Vector3 _r;
	DirectX::SimpleMath::Vector3 _u = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);;
	DirectX::SimpleMath::Vector3 _l = XMLoadFloat3(&dir);
	DirectX::SimpleMath::Vector3 _p = XMLoadFloat3(&pos);

	_l = XMVector3Normalize(_l);
	_r = XMVector3Normalize(XMVector3Cross(_l, _u));
	_u = XMVector3Cross(_l, _r);

	float _x = -XMVectorGetX(XMVector3Dot(_p, _r));
	float _y = -XMVectorGetX(XMVector3Dot(_p, _u));
	float _z = -XMVectorGetX(XMVector3Dot(_p, _l));

	
	m_ViewTM = DirectX::SimpleMath::Matrix(
		_r.x, _u.x, _l.x, 0.0f,
		_r.y, _u.y, _l.y, 0.0f,
		_r.z, _u.z, _l.z, 0.0f,
		_x, _y, _z, 1.0f);

	// 정사각형 광원에 대한 시야 및 화면 비율을 설정합니다.
	float fieldOfView = (float)XM_PI / 2.0f;
	float screenAspect = 1.0f;

	// 빛의 투영 행렬을 만듭니다.
	m_ProjTM = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 1.0f, 1000.0f);

	m_ShadowTM = m_ViewTM * m_ProjTM;

	m_ViewProjTM = XMMatrixMultiply(m_ViewTM, m_ProjTM);
}
