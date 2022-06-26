#include "pch.h"
#include "ReflectionProbeCamera.h"

#include "Transform.h"
#include "MeshRenderer.h"
#include "EngineDLL.h"
#include "Managers.h"
#include "IRenderer.h"
#include "IResourceManager.h"
#include "Managers.h"

#include "EngineDLL.h"

void CameraProbe::LookAT(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& target, const DirectX::SimpleMath::Vector3& worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_Position, pos);
	XMStoreFloat3(&m_Look, L);
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
}

void CameraProbe::SetLens(float fovY, float aspect, float zn, float zf)
{
	m_Proj = XMMatrixPerspectiveFovLH(fovY, aspect, zn, zf);
}

void CameraProbe::UpdateViewMatrix(const DirectX::SimpleMath::Matrix& world)
{
	DirectX::SimpleMath::Vector3 _r = m_Right;
	DirectX::SimpleMath::Vector3 _u = m_Up;
	DirectX::SimpleMath::Vector3 _l = m_Look;
	DirectX::SimpleMath::Vector3 _p = m_Position;

	_l = XMVector3Normalize(_l);
	_u = XMVector3Normalize(XMVector3Cross(_l, _r));
	_r = XMVector3Cross(_u, _l);

	float _x = -XMVectorGetX(XMVector3Dot(_p, _r));
	float _y = -XMVectorGetX(XMVector3Dot(_p, _u));
	float _z = -XMVectorGetX(XMVector3Dot(_p, _l));

	m_Right = _r;
	m_Up = _u;
	m_Look = _l;

	m_View = DirectX::SimpleMath::Matrix(
		m_Right.x, m_Up.x, m_Look.x, 0.0f,
		m_Right.y, m_Up.y, m_Look.y, 0.0f,
		m_Right.z, m_Up.z, m_Look.z, 0.0f,
		_x, _y, _z, 1.0f);
}

_DLL ReflectionProbeCamera::ReflectionProbeCamera()
	: ComponentBase(ComponentType::Etc)
	, m_ProbeIndex(0)
	, m_ReferenceDistance(10.0f)
	, m_IsBaking(false)
	, m_SaveData(new ReflectionProbeCamera_Save())
{
	m_pRenderer = DLLEngine::GetEngine()->GetIRenderer();
	m_SceneName =  DLLEngine::GetNowSceneName();

	// 여기서 리플렉션 프로브를 생성한다
	m_ProbeIndex = m_pRenderer->AddReflectionProbe();

	{
		// 맵 이름과 인덱스 초기화
		m_Maps[ENVIRONMENT_MAP] = 0; m_Maps[IRRADIANCE_MAP] = 0; m_Maps[PREFILTER_MAP] = 0;

		m_MapNames[ENVIRONMENT_MAP] = ENV_NAME + std::to_string(m_ProbeIndex);
		m_MapNames[IRRADIANCE_MAP] = IRR_NAME + std::to_string(m_ProbeIndex);
		m_MapNames[PREFILTER_MAP] = FILTER_NAME + std::to_string(m_ProbeIndex);
	}

}

_DLL ReflectionProbeCamera::~ReflectionProbeCamera()
{

}

void ReflectionProbeCamera::Start()
{
	m_pRenderer->SetReflectionBakedDDS(m_ProbeIndex, m_Maps[0], m_Maps[1], m_Maps[2]);

	CheckTargetDistance();
}

void ReflectionProbeCamera::Update(float dTime)
{
	float x, y, z = 0.0;
	x = m_pMyObject->GetTransform()->GetWorldPosition().x;
	y = m_pMyObject->GetTransform()->GetWorldPosition().y;
	z = m_pMyObject->GetTransform()->GetWorldPosition().z;

	XMFLOAT3 center(x, y, z);
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);

	// Look along each coordinate axis.
	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(x + 1.0f, y, z), // +X
		XMFLOAT3(x - 1.0f, y, z), // -X
		XMFLOAT3(x, y + 1.0f, z), // +Y
		XMFLOAT3(x, y - 1.0f, z), // -Y
		XMFLOAT3(x, y, z + 1.0f), // +Z
		XMFLOAT3(x, y, z - 1.0f),  // -Z
	};

	// Use world up vector (0,1,0) for all directions except +Y/-Y.  In these cases, we
	// are looking down +Y or -Y, so we need a different "up" vector.
	XMFLOAT3 ups[6] =
	{
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
		XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
		XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
		XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
		XMFLOAT3(0.0f, 1.0f, 0.0f),	 // -Z
	};


	for (unsigned int i = 0; i < CAMERA_FACE; i++)
	{
		m_CameraPorbe[i].LookAT(center, targets[i], ups[i]);
		m_CameraPorbe[i].SetLens(0.5f * DirectX::XM_PI, 1.0f, 0.1f, 1000.0f);
		m_CameraPorbe[i].UpdateViewMatrix(m_pMyObject->GetTransform()->GetWorld());
	}

	m_pRenderer->ReflectionProbeSetWorldTM(m_ProbeIndex, m_pMyObject->GetTransform()->GetWorld());
	BakingEnvironment();
}

void ReflectionProbeCamera::OnRender()
{

}

_DLL void ReflectionProbeCamera::BakeReflectionProbe()
{
	m_IsBaking = true;
}

_DLL void ReflectionProbeCamera::SetSceneIndex(const unsigned int idx)
{
	m_SceneIndex = idx;
}

_DLL void ReflectionProbeCamera::SetReferenceDistance(float dis)
{
	m_ReferenceDistance = dis;
}

_DLL void ReflectionProbeCamera::SetIsRender(bool val)
{
	m_IsRender = val;
}

_DLL bool ReflectionProbeCamera::GetIsRender()
{
	return m_IsRender;
}

void ReflectionProbeCamera::ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager)
{
	// 기존에 구워둔 DDS가 있다면 가져온다
	for (unsigned int i = 0; i < 3; i++)
	{
		//m_MapNames[i] = std::to_string(m_SceneIndex) + m_MapNames[i];
		m_MapNames[i] = m_SceneName + m_MapNames[i];
		m_Maps[i] = pReosureManager->GetIBLImageIndex(m_MapNames[i]);
	}
}

void ReflectionProbeCamera::BakingEnvironment()
{
	// 이 프로세스가 온렌더에 있어야하는 이유는?
	// 조명 연산을 받은 forward 형태로 그림을 그려야하기 때문?

	if (m_IsRender == true)
	{
		Managers::GetInstance()->SetRenderMode(REFLECTION_PROBE);

		for (unsigned int i = 0; i < CAMERA_FACE; i++)
		{
			// 리플렉션 프로브의 렌더타겟을 바인딩한다
			Managers::GetInstance()->GetCameraManager()->GetViewTM() = m_CameraPorbe[i].m_View;
			Managers::GetInstance()->GetCameraManager()->GetProjTM() = m_CameraPorbe[i].m_Proj;
			m_pRenderer->ReflectionProbeRenderTargetViewBind(m_ProbeIndex, i);

			Managers::GetInstance()->GetComponentSystem()->Render();
		}

		Managers::GetInstance()->SetRenderMode(DEFAULT);
	}
	

	if (m_IsBaking == false) return;
	// 이쯤에 리플렉션 프로브가 베이킹하도록 렌더러에 요청해야함
	// 위에 있는 프로세스는 단지 그림을 그릴 뿐, 베이킹 단계가 아님
	CheckTargetDistance();

	m_pRenderer->ReflectionProbeBaking(m_SceneIndex, m_SceneName, m_ProbeIndex);

	m_IsBaking = false;
}

void ReflectionProbeCamera::CheckTargetDistance()
{
	std::vector<ComponentBase*> _nowMeshRenderer;
	_nowMeshRenderer = Managers::GetInstance()->GetComponentSystem()->GetRenderComponent();

	for (auto& _meshRender : _nowMeshRenderer)
	{
		MeshRenderer* _targetObj = dynamic_cast<MeshRenderer*>(_meshRender);
		if (_targetObj != nullptr)
		{
			if (_targetObj->GetReflectionProbeIndex() == REFLECTION_PROBE_MAXNUM)
			{
				// 기본적으로 들어가는 IBL 이미지
				_targetObj->SetReflectionProbeIndex(m_ProbeIndex);
			}
			else
			{
				DirectX::SimpleMath::Vector3 _v1, _v2;

				_v1 = _targetObj->GetMyObject()->GetTransform()->m_WorldPosition;
				_v2 = m_pMyObject->GetTransform()->m_WorldPosition;
				float _targetDistance = DirectX::SimpleMath::Vector3::Distance(_v1, _v2);

				// 타겟 오브젝트의 거리가 미리 정한 기준 내에 있다면
				if (m_ReferenceDistance >= _targetDistance)
				{
					_targetObj->SetReflectionProbeIndex(m_ProbeIndex);
				}
			}
		}
	}
}

void ReflectionProbeCamera::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void ReflectionProbeCamera::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);
}

