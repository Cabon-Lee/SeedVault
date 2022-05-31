#include "pch.h"
#include "TransformAnimation.h"
#include "Transform.h"

TransformAnimation::TransformAnimation()
	: ComponentBase(ComponentType::Animation)
	, m_isPlay(false)
{
}

TransformAnimation::~TransformAnimation()
{
}

/// <summary>
/// 트랜스폼 애니메이션 초기화
///  : fps 설정, 키프레임 프레임 설정, 플레이타임 설정
/// </summary>
void TransformAnimation::Start()
{
	switch (m_FrameRate)
	{
	case eFrameRate::FPS24:
		m_fps = frameRate::FRAME_24;
		break;
	case eFrameRate::FPS30:
		m_fps = frameRate::FRAME_30;
		break;
	case eFrameRate::FPS60:
		m_fps = frameRate::FRAME_60;
		break;
	default:
		m_fps = frameRate::FRAME_30;
		break;
	}

	// 포문을 돌면서 Actor에 있는 키프레임에 접근을 하고
	// 키프레임마다 프레임을 설정해준다.
	SetKeyFrameInfo();

	// 가장긴시간을 가진 엑터의 재생시간을 플레이타임으로 놓는다
	SetPlayTime();

}

DirectX::SimpleMath::Matrix MatrixLerp(
	DirectX::SimpleMath::Matrix& easeInTM,
	DirectX::SimpleMath::Matrix& easeOutTM,
	float blend)
{
	DirectX::SimpleMath::Vector3 _size;
	DirectX::SimpleMath::Vector3 _easeInPos;
	DirectX::SimpleMath::Quaternion _easeInQuaternion;
	DirectX::SimpleMath::Vector3 _easeOutPos;
	DirectX::SimpleMath::Quaternion _easeOutQuaternion;

	easeInTM.Decompose(_size, _easeInQuaternion, _easeInPos);
	easeOutTM.Decompose(_size, _easeOutQuaternion, _easeOutPos);

	DirectX::SimpleMath::Vector3 _nowPos;
	DirectX::SimpleMath::Quaternion _nowQuaternion;

	_nowPos = DirectX::SimpleMath::Vector3::Lerp(_easeOutPos, _easeInPos, blend);
	_nowQuaternion = DirectX::SimpleMath::Quaternion::Slerp(_easeOutQuaternion, _easeInQuaternion, blend);

	return
		DirectX::SimpleMath::Matrix::CreateScale(1.0f) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(_nowQuaternion) *
		DirectX::SimpleMath::Matrix::CreateTranslation(_nowPos);
}


/// <summary>
/// 트랜스폼 애니메이션 업데이트
///  : 이전 키프레임의 TM과 현재 키프레임의 TM을 보간한 TM값을 사용해 애니메이션을 업데이트한다
/// </summary>
/// <param name="dTime"></param>
void TransformAnimation::Update(float dTime)
{
	if (m_isPlay == false)
	{
		return;
	}

	m_timeStack += dTime;
	
	// 현재프레임 = 진행된 시간 * fps
	unsigned int nowFrame = static_cast<unsigned int>(m_timeStack * m_fps);
	
	for (auto& _nowActor : m_actor_V)
	{
		// 엑터의 마지막프레임보다 현재프레임의 수가 더 높으면(더많이 진행됨 = 애니메이션 끝남)
		if (_nowActor->endFrame < nowFrame)
		{
			continue; // 이 액터는 건너뛴다
		}

		// 키프레임이 없거나 기본밖에 없을경우
		if (_nowActor->keyFrame_V.size() <= 1)
		{
			continue; // 이 액터는 건너뛴다
		}


		// 매 프레임마다 imgui에 있는 local pos, rot를 가져와서 originTM을 갱신시켜줌
		// 매 프레임마다 할 필요는 없긴한데...
		_nowActor->OriginTransformUpdata();

		// 키프레임 벡터에서 현재 키ㅋ프레임에대한 정보 얻어오기위해 인덱스를 정한다 
		unsigned int _currKey = _nowActor->currKeyframe;
		unsigned int _preKey = _currKey - 1;
		unsigned int _nextKey = _currKey + 1;

		// 알파값 :
		// 프레임(재생시간이 3초이고 60fps 라면, 0 ~ 180)	* (1 / 재생시간) * (1 / fps)
		// (지금 프레임 - 이전 키프레임의 프레임)			* (1 / 재생시간) * (1 / fps)
		float alpha = (nowFrame - _nowActor->keyFrame_V[_preKey].frame) * (1 / (_nowActor->keyFrame_V[_currKey].duration)) * (1 / m_fps);

		// 현재와 다음 키프레임 행렬을 보간
		DirectX::SimpleMath::Matrix _startMat = _nowActor->keyFrame_V[_preKey].keyMatrix;
		DirectX::SimpleMath::Matrix _endMat = _nowActor->keyFrame_V[_currKey].keyMatrix;
		DirectX::SimpleMath::Matrix _currLocalMatrix = MatrixLerp(_endMat, _startMat, alpha);

		// originTM은(오브젝트가 원래 있어야할 로컬TM, CurrentTM이 아님)
		// 보간한 행렬과 OriginTM을 곱해서 액터의 로컬TM을 만들고 이를 현재 액터의 LocalTM을 구함
		DirectX::SimpleMath::Matrix _nowLocalPos = 	_nowActor->originTM * _currLocalMatrix;

		_nowActor->pGameObject->GetTransform()->SetLocalTM(_nowLocalPos);
		
		if (nowFrame >= _nowActor->keyFrame_V[_currKey].frame)
		{
			_nowActor->currKeyframe++;

			// 현재 키프레임이 늘어났을때 인덱스보다 많다면
			if (_nowActor->keyFrame_V.size() <= _nowActor->currKeyframe)
			{
				// 키프레임은 항상 인덱스 1부터 시작한다
				_nowActor->currKeyframe = 1;
			}
		}
	}
	
	/// 플레이타임보다 쌓인 시간이 많으면 애니메이션이 종료됨
	if (m_timeStack > m_playTime)
	{
		m_timeStack = 0;
		switch (m_playType)
		{
		case ePlayType::Loop:
			return;
		case ePlayType::Once:
			m_isPlay = false;
			return;
		}
	}
}

/// <summary>
/// 플레이 방식 설정
/// </summary>
/// <param name="playType"></param>
void TransformAnimation::SetPlayType(ePlayType playType)
{
	m_playType = playType;
}

/// <summary>
/// FPS 설정
/// </summary>
/// <param name="frameRate"></param>
void TransformAnimation::SetFrameRate(eFrameRate frameRate)
{
	m_FrameRate = frameRate;
}

/// <summary>
/// 액터 추가
/// </summary>
/// <param name="gameObject"></param>
void TransformAnimation::AddActor(GameObject* gameObject)
{
	Actor* actor = new Actor(gameObject);
	m_actor_V.push_back(actor);
}

/// <summary>
/// 외부에서 액터에 접근할 수 있게하는 함수
/// </summary>
/// <param name="gameObjectName">액터의 오브젝트의 이름</param>
/// <returns>m_actor_V의 액터</returns>
Actor* TransformAnimation::GetActor(std::string gameObjectName)
{
	for(Actor* actor : m_actor_V)
	{
		if (std::string(actor->pGameObject->GetObjectName())== gameObjectName)
		{
			return actor;
		}
	}

	return nullptr;
}

/// <summary>
/// 애니메이션 리셋
/// </summary>
void TransformAnimation::Reset()
{
	m_timeStack = 0;
	m_isPlay = false;
	for (Actor* actor : m_actor_V)
	{
		actor->currKeyframe = 1;
	}
}

/// <summary>
/// 애니메이션 플레이
/// </summary>
void TransformAnimation::Play()
{
	m_isPlay = true;
}

/// <summary>
/// 액터의 키프레임들을 프레임을 계산해서 넣어준다
/// 액터의 마지막 프레임이 몇 프레임인지도 계산해준다
/// </summary>
void TransformAnimation::SetKeyFrameInfo()
{
	if (m_actor_V.size() <= 0)
	{
		return;
	}

	for (int i = 0; i < m_actor_V.size(); ++i)
	{
		unsigned int _accumulateTime = 0;
		for (int j = 0; j < m_actor_V[i]->keyFrame_V.size(); ++j)
		{
			_accumulateTime += m_actor_V[i]->keyFrame_V[j].duration;
			m_actor_V[i]->keyFrame_V[j].frame = _accumulateTime * m_fps;
		}

		m_actor_V[i]->endFrame *= m_fps;
	}
}

/// <summary>
/// 애니메이션의 플레이 시간을 정한다
/// </summary>
void TransformAnimation::SetPlayTime()
{
	// 엑터들의 총 재생시간을 알아낸다
	if (m_actor_V.size() <= 0)
	{
		m_playTime = 0;
		return;
	}

	int* _actorPlayTime = new int[m_actor_V.size()];

	for (int i = 0; i < m_actor_V.size(); ++i)
	{
		_actorPlayTime[i] = 0.0f;

		for (int j = 0; j < m_actor_V[i]->keyFrame_V.size(); ++j)
		{
			_actorPlayTime[i] += m_actor_V[i]->keyFrame_V[j].duration;
		}
	}

	// 가장 긴 엑터의 길이를 총재생시간으로 정한다
	m_playTime = _actorPlayTime[0];
	for (int i = 0; i + 1 < m_actor_V.size(); ++i)
	{
		if (_actorPlayTime[i] < _actorPlayTime[i + 1])
		{
			m_playTime = _actorPlayTime[i + 1];
		}
	}
}



/// <summary>
/// 액터 생성자
///  : 게임오브젝트를 받아서 엑터를 만들고 0번째 키프레임을 하나 추가한다
/// </summary>
/// <param name="pGO"></param>
Actor::Actor(GameObject* pGO)
	: currKeyframe(1), endFrame(0)
{
	pGameObject = pGO;
	KeyFrame firstOne(0.0f, 0, pGameObject->GetTransform()->GetLocal());
	keyFrame_V.push_back(firstOne);
}

/// <summary>
/// 키프레임을 추가
/// </summary>
/// <param name="dur">키프레임의 시간</param>
/// <param name="pos">키프레임 트랜지션 매트릭스를 만들 벡터</param>
_DLL void Actor::AddKeyFrame(float dur, DirectX::SimpleMath::Vector3 pos)
{
	KeyFrame _added(dur, 0);
	_added.TransformUpdate(pGameObject->GetTransform(), pos);
	endFrame += dur;

	keyFrame_V.push_back(_added);
};

/// <summary>
/// 키프레임을 추가
/// </summary>
/// <param name="dur">키프레임의 시간</param>
/// <param name="pos">키프레임 트랜지션 매트릭스를 만들 벡터</param>
/// <param name="rot">키프레임 로테이션 매트릭스를 만들 벡터/param>
_DLL void Actor::AddKeyFrame(float dur, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 rot)
{
	// 오일러 회전 -> 쿼터니언 회전으로 전환
	DirectX::SimpleMath::Vector3 _EulerAngles;
	//currRot += rot;
	_EulerAngles = rot;
	
	// Degree -> Radian
	float _radianX = _EulerAngles.x * MathHelper::Pi / 180;
	float _radianY = _EulerAngles.y * MathHelper::Pi / 180;
	float _radianZ = _EulerAngles.z * MathHelper::Pi / 180;

	// 쿼터니언을 만들고 계산된 쿼터니언으로 매트릭스를 세팅
	DirectX::SimpleMath::Quaternion _quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(_radianY, _radianX, _radianZ);
	DirectX::SimpleMath::Vector3 _scale = pGameObject->GetTransform()->GetScale();
	
	DirectX::SimpleMath::Matrix scaleMatrix = XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(_quat);
	DirectX::SimpleMath::Matrix translateMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

	DirectX::SimpleMath::Matrix LocalMat = scaleMatrix * rotationMatrix * translateMatrix;

	KeyFrame _added(dur, 0);
	_added.TransformUpdate(pGameObject->GetTransform(), pos, rot);
	endFrame += dur;

	keyFrame_V.push_back(_added);
};

/// <summary>
/// 키프레임을 추가
/// </summary>
/// <param name="dur">키프레임의 시간</param>
/// <param name="mat">키프레임의 매트릭스</param>
_DLL void Actor::AddKeyFrame(float dur, DirectX::SimpleMath::Matrix mat)
{
	KeyFrame _added(dur, 0, mat);
	endFrame += dur;

	keyFrame_V.push_back(_added);
}
void Actor::OriginTransformUpdata()
{
	// 오리진 키프레임임
	DirectX::SimpleMath::Matrix _newTM =
	DirectX::SimpleMath::Matrix::CreateScale(pGameObject->GetTransform()->m_Scale) *
	DirectX::SimpleMath::Matrix::CreateFromQuaternion(pGameObject->GetTransform()->m_Rotation) *
	DirectX::SimpleMath::Matrix::CreateTranslation(pGameObject->GetTransform()->m_Position);
	originTM = _newTM;

};

void KeyFrame::TransformUpdate(Transform* transform)
{
	scale = transform->GetScale();
	rotation = transform->GetRotation();
	position = transform->GetWorldPosition();

	// 오일러 회전 -> 쿼터니언 회전으로 전환
	// Degree -> Radian
	float _radianX = changeRotation.x * MathHelper::Pi / 180;
	float _radianY = changeRotation.y * MathHelper::Pi / 180;
	float _radianZ = changeRotation.z * MathHelper::Pi / 180;

	// 쿼터니언을 만들고 계산된 쿼터니언으로 매트릭스를 세팅
	DirectX::SimpleMath::Quaternion _quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(_radianY, _radianX, _radianZ);

	DirectX::SimpleMath::Matrix scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation + _quat);
	DirectX::SimpleMath::Matrix translateMatrix = DirectX::XMMatrixTranslation(position.x + changePosition.x, position.y + changePosition.y, position.z + changePosition.z);

	keyMatrix = scaleMatrix * rotationMatrix * translateMatrix;
}

void KeyFrame::TransformUpdate(Transform* transform, DirectX::SimpleMath::Vector3 pos)
{
	/// 임구이랑 같이 쓰기 위해 필요한것
	scale = transform->GetScale();
	rotation = transform->GetRotation();
	position = transform->GetWorld().Translation();

	changeScale = Vector3();
	changeRotation = Vector3();
	changePosition = pos;
	
	//transform->SetScale(transform->GetScale());
	//transform->SetRotation(transform->GetRotation());
	//transform->SetPosition(transform->GetLocalPosition() + changePosition);

	/// 실제 업데이트가 아닌 키프레임에 넣어주기 위함
	//DirectX::SimpleMath::Matrix scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	//DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
	DirectX::SimpleMath::Matrix translateMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	keyMatrix = translateMatrix;
}

void KeyFrame::TransformUpdate(Transform* transform, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 rot)
{
	scale = transform->GetScale();
	rotation = transform->GetRotation();
	position = transform->GetWorldPosition();

	changeScale = Vector3();
	changeRotation = rot;
	changePosition = pos;

	// 오일러 회전 -> 쿼터니언 회전으로 전환
	// Degree -> Radian
	float _radianX = changeRotation.x * MathHelper::Pi / 180;
	float _radianY = changeRotation.y * MathHelper::Pi / 180;
	float _radianZ = changeRotation.z * MathHelper::Pi / 180;

	// 쿼터니언을 만들고 계산된 쿼터니언으로 매트릭스를 세팅
	DirectX::SimpleMath::Quaternion _quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(_radianY, _radianX, _radianZ);

	DirectX::SimpleMath::Matrix scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation + _quat);
	DirectX::SimpleMath::Matrix translateMatrix = XMMatrixTranslation(position.x + changePosition.x, position.y + changePosition.y, position.z + changePosition.z);

	keyMatrix = scaleMatrix * rotationMatrix * translateMatrix;
}

