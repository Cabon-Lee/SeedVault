#include "pch.h"
#include "PlayerController.h"
#include "CameraBehavior.h"

CameraBehavior::CameraBehavior()
	: ComponentBase(ComponentType::GameLogic)
	, m_PlayerController(nullptr)

	, m_ZoomInPosition(nullptr)
	, m_Camera(nullptr)

	, m_ZoomInFov(0.0f)
	, m_ZoomOutFov(0.0f)
	, m_CurrentFov(0.0f)

	, m_CameraSpeed(7.0f)

	, m_CurrentZoomInPos(Vector3::Zero)
	, m_CurrentCameraDefaultPos(Vector3::Zero)

	, m_StandZoomInPos(nullptr)
	, m_StandCameraDefaultPos(Vector3::Zero)

	, m_CrouchZoomInPos(Vector3::Zero)
	, m_CrouchCameraDefaultPos(Vector3::Zero)

	, m_SaveData(new CameraBehavior_Save())
{
}

CameraBehavior::~CameraBehavior()
{
}

void CameraBehavior::Start()
{
	// 플레이어 컨트롤러 연결
	GameObject* _player = DLLEngine::FindGameObjectByName("Player");
	if (_player != nullptr)
	{
		m_PlayerController = _player->GetComponent<PlayerController>();
	}
	assert(m_PlayerController != nullptr);

	// 메인 카메라 컴포넌트 연결
	GameObject* mainCameraObj = DLLEngine::FindGameObjectByName("MainCamera");
	assert(mainCameraObj != nullptr);

	if (m_ZoomInPosition == nullptr)
	{
		// ZoomInPosition 할당
		GameObject* _ZoomInPosObj = DLLEngine::FindGameObjectByName("ZoomInPosition");
		assert(_ZoomInPosObj != nullptr);

		m_ZoomInPosition = _ZoomInPosObj->m_Transform;

		m_StandZoomInPos = &m_ZoomInPosition->m_Position;
	}

	// 줌인/ 줌아웃 포지션 세팅
	SetZoomInPosition();
	m_StandCameraDefaultPos = m_Transform->GetLocalPosition();

	m_CrouchCameraDefaultPos = m_StandCameraDefaultPos;
	m_CrouchCameraDefaultPos += { 0.0f, -0.3f, 0.0f };

	m_Camera = mainCameraObj->GetComponent<Camera>();

	m_ZoomOutFov = m_Camera->GetFovY();
	m_ZoomInFov = m_ZoomOutFov - 0.1f;
	m_CurrentFov = m_ZoomOutFov;

	// 초기 줌인/ 디폴트 포지션 세팅
	m_CurrentZoomInPos = *m_StandZoomInPos;
	m_CurrentCameraDefaultPos = m_StandCameraDefaultPos;

}

void CameraBehavior::Update(float dTime)
{
	// 플레이어 자세(Stand or Crouch) 체크
	if (m_PlayerController->m_bCrouch)
	{
		m_CurrentZoomInPos = m_CrouchZoomInPos;
		m_CurrentCameraDefaultPos = m_CrouchCameraDefaultPos;
	}
	else
	{
		m_CurrentZoomInPos = *m_StandZoomInPos;
		m_CurrentCameraDefaultPos = m_StandCameraDefaultPos;
	}

	// 플레이어가 조준중이면
	if (m_PlayerController->m_bAim)
	{
		// 카메라 줌인
		CameraZoomIn();
		//CA_TRACE("Zoom In");
	}

	// 비 조준
	else
	{
		// 줌 아웃
		CameraZoomOut();

		/// 카메라 충돌(Bounding Camera)
		static GameObject* _player = DLLEngine::FindGameObjectByName("Player");

		const SimpleMath::Vector3& _ori = m_ZoomInPosition->m_WorldPosition;
		//SimpleMath::Vector3 _dir = -m_ZoomInPosition->m_WorldTM.Forward();
		SimpleMath::Vector3 _dir = { m_Transform->m_WorldPosition.x, m_Transform->m_WorldPosition.y /*- 0.5f*/, m_Transform->m_WorldPosition.z - 0.5f};
		_dir -= _ori;
		_dir.Normalize();

		float _distZoomInPosToCamera = SimpleMath::Vector3::Distance(_ori, m_Transform->m_WorldPosition);

		RaycastHit _hit;
		_hit = *DLLEngine::CheckRaycastHit(_ori, _dir , _distZoomInPosToCamera);

		if (_hit.actor != nullptr
			//&& _hit.actor->GetMyObject()->GetTag() == "Wall"
			)
		{
			CA_TRACE("카메라 충돌");

			float _distPlayerToHitPos = Vector3::Distance(_player->m_Transform->m_WorldPosition, _hit.position);
			//if (_distPlayerToHitPos < 2.0f)
			{
				SimpleMath::Vector3 pos = m_CurrentZoomInPos - m_Transform->m_WorldTM.Forward() * 0.2f;

				//m_Transform->m_Position = Vector3::Lerp(m_Transform->GetLocalPosition(), pos, CL::Input::s_DeltaTime * m_CameraSpeed);
				m_Transform->m_Position = Vector3::Lerp(m_Transform->GetLocalPosition(), m_CurrentZoomInPos, CL::Input::s_DeltaTime * m_CameraSpeed);
			}
		}
	}
}

void CameraBehavior::OnRender()
{
}

void CameraBehavior::SetZoomInPosition()
{
	// 카메라 줌 인/아웃 변수	
	// 스탠드 줌인/ 디폴트 포지션 세팅
	//m_StandZoomInPos = &m_ZoomInPosition->m_Position; // -> 스탠드 줌인포지션은 포인터로 ZoomInPosition 오브젝트를 가리키므로 매번 갱신할 필요 없음

	// 크라우치 줌인/ 디폴트 포지션 세팅
	m_CrouchZoomInPos = *m_StandZoomInPos;
	m_CrouchZoomInPos += { 0.0f, -0.2f, 0.0f };


}

void CameraBehavior::CameraZoomIn()
{
	// 카메라 포지션을 Zoom In Position 으로 보간
	m_Transform->m_Position = Vector3::Lerp(m_Transform->m_Position, m_CurrentZoomInPos, CL::Input::s_DeltaTime * m_CameraSpeed);

	if (m_CurrentFov > m_ZoomInFov)
	{
		m_CurrentFov -= CL::Input::s_DeltaTime;
	}

	m_Camera->SetFovY(m_CurrentFov);
}

void CameraBehavior::CameraZoomOut()
{
	// 디폴트 포지션으로 보간
	m_Transform->m_Position = Vector3::Lerp(m_Transform->GetLocalPosition(), m_CurrentCameraDefaultPos, CL::Input::s_DeltaTime * m_CameraSpeed);


	if (m_CurrentFov < m_ZoomOutFov)
	{
		m_CurrentFov += CL::Input::s_DeltaTime;
	}

	m_Camera->SetFovY(m_CurrentFov);
}

void CameraBehavior::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_ZoomInPosition = m_ZoomInPosition->GetComponetId();

	m_SaveData->m_ZoomInFov = m_ZoomInFov;
	m_SaveData->m_ZoomOutFov = m_ZoomOutFov;
	m_SaveData->m_CurrentFov = m_CurrentFov;

	m_SaveData->m_CameraSpeed = m_CameraSpeed;

	m_SaveData->m_CurrentZoomInPos = &m_CurrentZoomInPos;
	m_SaveData->m_CurrentCameraDefaultPos = &m_CurrentCameraDefaultPos;

	//m_SaveData->m_StandZoomInPos = m_StandZoomInPos;
	m_SaveData->m_StandCameraDefaultPos = &m_StandCameraDefaultPos;

	m_SaveData->m_CrouchZoomInPos = &m_CrouchZoomInPos;
	m_SaveData->m_CrouchCameraDefaultPos = &m_CrouchCameraDefaultPos;

	m_SaveData->m_pObject = m_pMyObject->GetGameObjectId();
	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void CameraBehavior::LoadData()
{
	m_SaveData->m_CurrentZoomInPos = &m_CurrentZoomInPos;
	m_SaveData->m_CurrentCameraDefaultPos = &m_CurrentCameraDefaultPos;

	//m_SaveData->m_StandZoomInPos = m_StandZoomInPos;
	m_SaveData->m_StandCameraDefaultPos = &m_StandCameraDefaultPos;

	m_SaveData->m_CrouchZoomInPos = &m_CrouchZoomInPos;
	m_SaveData->m_CrouchCameraDefaultPos = &m_CrouchCameraDefaultPos;

	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	m_ZoomInFov = m_SaveData->m_ZoomInFov;
	m_ZoomOutFov = m_SaveData->m_ZoomOutFov;
	m_CurrentFov = m_SaveData->m_CurrentFov;

	m_CameraSpeed = m_SaveData->m_CameraSpeed;

}

void CameraBehavior::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{
	if (m_SaveData->m_ZoomInPosition != NULL)
	{
		m_ZoomInPosition = dynamic_cast<Transform*>(component->at(m_SaveData->m_ZoomInPosition));
		m_StandZoomInPos = &m_ZoomInPosition->m_Position;

	}
}
