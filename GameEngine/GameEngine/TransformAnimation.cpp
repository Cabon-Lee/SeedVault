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
/// Ʈ������ �ִϸ��̼� �ʱ�ȭ
///  : fps ����, Ű������ ������ ����, �÷���Ÿ�� ����
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

	// ������ ���鼭 Actor�� �ִ� Ű�����ӿ� ������ �ϰ�
	// Ű�����Ӹ��� �������� �������ش�.
	SetKeyFrameInfo();

	// �����ð��� ���� ������ ����ð��� �÷���Ÿ������ ���´�
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
/// Ʈ������ �ִϸ��̼� ������Ʈ
///  : ���� Ű�������� TM�� ���� Ű�������� TM�� ������ TM���� ����� �ִϸ��̼��� ������Ʈ�Ѵ�
/// </summary>
/// <param name="dTime"></param>
void TransformAnimation::Update(float dTime)
{
	if (m_isPlay == false)
	{
		return;
	}

	m_timeStack += dTime;
	
	// ���������� = ����� �ð� * fps
	unsigned int nowFrame = static_cast<unsigned int>(m_timeStack * m_fps);
	
	for (auto& _nowActor : m_actor_V)
	{
		// ������ �����������Ӻ��� ������������ ���� �� ������(������ ����� = �ִϸ��̼� ����)
		if (_nowActor->endFrame < nowFrame)
		{
			continue; // �� ���ʹ� �ǳʶڴ�
		}

		// Ű�������� ���ų� �⺻�ۿ� �������
		if (_nowActor->keyFrame_V.size() <= 1)
		{
			continue; // �� ���ʹ� �ǳʶڴ�
		}


		// �� �����Ӹ��� imgui�� �ִ� local pos, rot�� �����ͼ� originTM�� ���Ž�����
		// �� �����Ӹ��� �� �ʿ�� �����ѵ�...
		_nowActor->OriginTransformUpdata();

		// Ű������ ���Ϳ��� ���� Ű�������ӿ����� ���� ���������� �ε����� ���Ѵ� 
		unsigned int _currKey = _nowActor->currKeyframe;
		unsigned int _preKey = _currKey - 1;
		unsigned int _nextKey = _currKey + 1;

		// ���İ� :
		// ������(����ð��� 3���̰� 60fps ���, 0 ~ 180)	* (1 / ����ð�) * (1 / fps)
		// (���� ������ - ���� Ű�������� ������)			* (1 / ����ð�) * (1 / fps)
		float alpha = (nowFrame - _nowActor->keyFrame_V[_preKey].frame) * (1 / (_nowActor->keyFrame_V[_currKey].duration)) * (1 / m_fps);

		// ����� ���� Ű������ ����� ����
		DirectX::SimpleMath::Matrix _startMat = _nowActor->keyFrame_V[_preKey].keyMatrix;
		DirectX::SimpleMath::Matrix _endMat = _nowActor->keyFrame_V[_currKey].keyMatrix;
		DirectX::SimpleMath::Matrix _currLocalMatrix = MatrixLerp(_endMat, _startMat, alpha);

		// originTM��(������Ʈ�� ���� �־���� ����TM, CurrentTM�� �ƴ�)
		// ������ ��İ� OriginTM�� ���ؼ� ������ ����TM�� ����� �̸� ���� ������ LocalTM�� ����
		DirectX::SimpleMath::Matrix _nowLocalPos = 	_nowActor->originTM * _currLocalMatrix;

		_nowActor->pGameObject->GetTransform()->SetLocalTM(_nowLocalPos);
		
		if (nowFrame >= _nowActor->keyFrame_V[_currKey].frame)
		{
			_nowActor->currKeyframe++;

			// ���� Ű�������� �þ���� �ε������� ���ٸ�
			if (_nowActor->keyFrame_V.size() <= _nowActor->currKeyframe)
			{
				// Ű�������� �׻� �ε��� 1���� �����Ѵ�
				_nowActor->currKeyframe = 1;
			}
		}
	}
	
	/// �÷���Ÿ�Ӻ��� ���� �ð��� ������ �ִϸ��̼��� �����
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
/// �÷��� ��� ����
/// </summary>
/// <param name="playType"></param>
void TransformAnimation::SetPlayType(ePlayType playType)
{
	m_playType = playType;
}

/// <summary>
/// FPS ����
/// </summary>
/// <param name="frameRate"></param>
void TransformAnimation::SetFrameRate(eFrameRate frameRate)
{
	m_FrameRate = frameRate;
}

/// <summary>
/// ���� �߰�
/// </summary>
/// <param name="gameObject"></param>
void TransformAnimation::AddActor(GameObject* gameObject)
{
	Actor* actor = new Actor(gameObject);
	m_actor_V.push_back(actor);
}

/// <summary>
/// �ܺο��� ���Ϳ� ������ �� �ְ��ϴ� �Լ�
/// </summary>
/// <param name="gameObjectName">������ ������Ʈ�� �̸�</param>
/// <returns>m_actor_V�� ����</returns>
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
/// �ִϸ��̼� ����
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
/// �ִϸ��̼� �÷���
/// </summary>
void TransformAnimation::Play()
{
	m_isPlay = true;
}

/// <summary>
/// ������ Ű�����ӵ��� �������� ����ؼ� �־��ش�
/// ������ ������ �������� �� ������������ ������ش�
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
/// �ִϸ��̼��� �÷��� �ð��� ���Ѵ�
/// </summary>
void TransformAnimation::SetPlayTime()
{
	// ���͵��� �� ����ð��� �˾Ƴ���
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

	// ���� �� ������ ���̸� ������ð����� ���Ѵ�
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
/// ���� ������
///  : ���ӿ�����Ʈ�� �޾Ƽ� ���͸� ����� 0��° Ű�������� �ϳ� �߰��Ѵ�
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
/// Ű�������� �߰�
/// </summary>
/// <param name="dur">Ű�������� �ð�</param>
/// <param name="pos">Ű������ Ʈ������ ��Ʈ������ ���� ����</param>
_DLL void Actor::AddKeyFrame(float dur, DirectX::SimpleMath::Vector3 pos)
{
	KeyFrame _added(dur, 0);
	_added.TransformUpdate(pGameObject->GetTransform(), pos);
	endFrame += dur;

	keyFrame_V.push_back(_added);
};

/// <summary>
/// Ű�������� �߰�
/// </summary>
/// <param name="dur">Ű�������� �ð�</param>
/// <param name="pos">Ű������ Ʈ������ ��Ʈ������ ���� ����</param>
/// <param name="rot">Ű������ �����̼� ��Ʈ������ ���� ����/param>
_DLL void Actor::AddKeyFrame(float dur, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 rot)
{
	// ���Ϸ� ȸ�� -> ���ʹϾ� ȸ������ ��ȯ
	DirectX::SimpleMath::Vector3 _EulerAngles;
	//currRot += rot;
	_EulerAngles = rot;
	
	// Degree -> Radian
	float _radianX = _EulerAngles.x * MathHelper::Pi / 180;
	float _radianY = _EulerAngles.y * MathHelper::Pi / 180;
	float _radianZ = _EulerAngles.z * MathHelper::Pi / 180;

	// ���ʹϾ��� ����� ���� ���ʹϾ����� ��Ʈ������ ����
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
/// Ű�������� �߰�
/// </summary>
/// <param name="dur">Ű�������� �ð�</param>
/// <param name="mat">Ű�������� ��Ʈ����</param>
_DLL void Actor::AddKeyFrame(float dur, DirectX::SimpleMath::Matrix mat)
{
	KeyFrame _added(dur, 0, mat);
	endFrame += dur;

	keyFrame_V.push_back(_added);
}
void Actor::OriginTransformUpdata()
{
	// ������ Ű��������
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

	// ���Ϸ� ȸ�� -> ���ʹϾ� ȸ������ ��ȯ
	// Degree -> Radian
	float _radianX = changeRotation.x * MathHelper::Pi / 180;
	float _radianY = changeRotation.y * MathHelper::Pi / 180;
	float _radianZ = changeRotation.z * MathHelper::Pi / 180;

	// ���ʹϾ��� ����� ���� ���ʹϾ����� ��Ʈ������ ����
	DirectX::SimpleMath::Quaternion _quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(_radianY, _radianX, _radianZ);

	DirectX::SimpleMath::Matrix scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation + _quat);
	DirectX::SimpleMath::Matrix translateMatrix = DirectX::XMMatrixTranslation(position.x + changePosition.x, position.y + changePosition.y, position.z + changePosition.z);

	keyMatrix = scaleMatrix * rotationMatrix * translateMatrix;
}

void KeyFrame::TransformUpdate(Transform* transform, DirectX::SimpleMath::Vector3 pos)
{
	/// �ӱ��̶� ���� ���� ���� �ʿ��Ѱ�
	scale = transform->GetScale();
	rotation = transform->GetRotation();
	position = transform->GetWorld().Translation();

	changeScale = Vector3();
	changeRotation = Vector3();
	changePosition = pos;
	
	//transform->SetScale(transform->GetScale());
	//transform->SetRotation(transform->GetRotation());
	//transform->SetPosition(transform->GetLocalPosition() + changePosition);

	/// ���� ������Ʈ�� �ƴ� Ű�����ӿ� �־��ֱ� ����
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

	// ���Ϸ� ȸ�� -> ���ʹϾ� ȸ������ ��ȯ
	// Degree -> Radian
	float _radianX = changeRotation.x * MathHelper::Pi / 180;
	float _radianY = changeRotation.y * MathHelper::Pi / 180;
	float _radianZ = changeRotation.z * MathHelper::Pi / 180;

	// ���ʹϾ��� ����� ���� ���ʹϾ����� ��Ʈ������ ����
	DirectX::SimpleMath::Quaternion _quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(_radianY, _radianX, _radianZ);

	DirectX::SimpleMath::Matrix scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation + _quat);
	DirectX::SimpleMath::Matrix translateMatrix = XMMatrixTranslation(position.x + changePosition.x, position.y + changePosition.y, position.z + changePosition.z);

	keyMatrix = scaleMatrix * rotationMatrix * translateMatrix;
}

