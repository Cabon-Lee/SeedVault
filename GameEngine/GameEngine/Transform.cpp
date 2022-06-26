#include "pch.h"
#include <math.h>
#include "Managers.h"
#include "PhysicsActor.h"
#include "Transform.h"

Transform::Transform()
	: ComponentBase(ComponentType::Etc)
	, m_Scale(1.0f, 1.0f, 1.0f)
	, m_Rotation(0.0f, 0.0f, 0.0f, 1.0f)
	, m_Position(0.0f, 0.0f, 0.0f)
	, m_pParent(nullptr)
	, m_EulerAngles(0.0f, 0.0f, 0.0f)
	, m_SaveData(new Transform_Save)
{

	m_WorldTM =
		DirectX::SimpleMath::Matrix::CreateScale(m_Scale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_Position);

	//m_LocalTM = m_WorldTM; // 생성시에는 부모가 없는 상태이므로 월드좌표가 기준
}

Transform::~Transform()
{
}

const DirectX::SimpleMath::Vector3& Transform::GetScale() const
{
	return m_Scale;
}

void Transform::SetScale(const DirectX::SimpleMath::Vector3& scl)
{
	m_Scale = scl;

	// 피직스 엑터가 있으면 피직스 데이터(트랜스폼)도 동기화한다.
	if (m_pMyObject->GetComponent<PhysicsActor>())
	{
		Managers::GetInstance()->GetPhysicsEngine()->SetPhysicsTransform(this);
	}
}

const DirectX::SimpleMath::Quaternion& Transform::GetRotation() const
{
	return m_Rotation;
}

void Transform::SetRotation(const DirectX::SimpleMath::Quaternion& rot)
{
	m_Rotation = rot;

	// Modified by 최 요 환
	// SetRotation() 좀 써보자!!
	// 파라미터로 받은 쿼터니언(rot)로 m_RotationTM 을 수정
	m_RotationTM = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation);

	m_EulerAngles = QuaternionToEuler(m_Rotation);   // 쿼터니언 값을 오일러로 변환해서 저장(버그 있음..)
}

// Added by 최 요 환
_DLL void Transform::SetRotationFromVec(const DirectX::SimpleMath::Vector2& rot)
{
	// 각 축기준(World 기준)으로 세팅한다

	// Added By 최 요 환
	m_EulerAngles.x = rot.x;
	m_EulerAngles.y = rot.y;

	SetRotationFromVec(m_EulerAngles);
}

// Added by 최 요 환
_DLL void Transform::SetRotationFromVec(const DirectX::SimpleMath::Vector3& rot)
{
	// 각 축기준(World 기준)으로 세팅한다

	// Added By 최 요 환
	m_EulerAngles = rot;
	m_EulerAngles = NormalizeAngles(m_EulerAngles);	// 0 ~ 360 사이로 제한

	// Degree -> Radian
	float _radianX = m_EulerAngles.x * MathHelper::Pi / 180;
	float _radianY = m_EulerAngles.y * MathHelper::Pi / 180;
	float _radianZ = m_EulerAngles.z * MathHelper::Pi / 180;

	// 쿼터니언을 만들고 계산된 쿼터니언으로 매트릭스를 세팅
	DirectX::SimpleMath::Quaternion _quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(_radianY, _radianX, _radianZ);

	m_Rotation = _quat;
	m_RotationTM = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation);

	// 피직스 액터가 오브젝트에 있으면
	if (m_pMyObject->GetComponent<PhysicsActor>())
	{
		// 피직스 씬의 액터에게 변경된 각도(트랜스폼) 설정해줌
		Managers::GetInstance()->GetPhysicsEngine()->SetPhysicsTransform(m_Transform);
	}
	
}

const DirectX::SimpleMath::Vector3& Transform::GetLocalPosition() const
{
	return m_Position;
}


_DLL void Transform::SetPosition(const DirectX::SimpleMath::Vector3& pos)
{
	m_Position = pos;

	m_LocalTM =
		DirectX::SimpleMath::Matrix::CreateScale(m_Scale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_Position);

	// 부모자식 관계 업데이트
	UpdateNodeTM();

	m_WorldPosition = m_WorldTM.Translation();

	// 피직스 엑터가 있으면 피직스 데이터(트랜스폼)도 동기화한다.
	if (m_pMyObject->GetComponent<PhysicsActor>())
	{
		Managers::GetInstance()->GetPhysicsEngine()->SetPhysicsTransform(this);
	}
}

_DLL const DirectX::SimpleMath::Vector3& Transform::GetWorldPosition() const
{
	return m_WorldPosition;
}

_DLL void Transform::MoveForwardOnXZ(float val)
{
	DirectX::SimpleMath::Vector3 _xz = { m_RotationTM.Forward().x, 0, m_RotationTM.Forward().z };
	_xz.Normalize();
	m_Position += _xz * val;



}

// 앞으로 나아가기위해 '회전된' Offset을 더해줌
_DLL void Transform::MoveForward(float val)
{
	DirectX::SimpleMath::Vector3 _forward = m_RotationTM.Forward();
	_forward.Normalize();
	m_Position += _forward * val; // simpleMath의 좌표계는 rightHand계라서 반대로 제공
}

_DLL void Transform::MoveSide(float val)
{
	DirectX::SimpleMath::Vector3 _side = m_RotationTM.Right();
	_side.Normalize();
	m_Position += _side * val;
}

// 회전하기 위한 함수들
// Modified by 최 요 환
_DLL void Transform::RotateY(float val)
{
	//(구)
	//DirectX::SimpleMath::Matrix _rotTM = DirectX::XMMatrixRotationAxis({ 0.0f, 1.0f, 0.0f }, val); // Y축으로 각도만큼 회전행렬 만듬
	//m_RotationTM *= _rotTM;                                     // 회전 시켜줌
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// Modified by 최 요 환
	m_EulerAngles.y += val;

	// Degree -> Radian
	float _radianX = m_EulerAngles.x * MathHelper::Pi / 180;
	float _radianY = m_EulerAngles.y * MathHelper::Pi / 180;
	float _radianZ = m_EulerAngles.z * MathHelper::Pi / 180;

	// 쿼터니언을 만들고 계산된 쿼터니언으로 매트릭스를 세팅
	DirectX::SimpleMath::Quaternion _quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(_radianY, _radianX, _radianZ);

	m_Rotation = _quat;

	m_RotationTM = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation);
}

_DLL void Transform::Pitch(float val)
{
	// (구)
	//DirectX::SimpleMath::Matrix _rotTM = DirectX::XMMatrixRotationAxis(m_LocalTM.Right(), val); // 시야의 오른쪽 축으로 각도만큼 회전행렬 만듬
	//m_RotationTM *= _rotTM;                                        // 회전 시켜줌
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// Modified By 최 요 환
	m_EulerAngles.x += val;

	// Degree -> Radian
	float _radianX = m_EulerAngles.x * MathHelper::Pi / 180;
	float _radianY = m_EulerAngles.y * MathHelper::Pi / 180;
	float _radianZ = m_EulerAngles.z * MathHelper::Pi / 180;

	// 쿼터니언을 만들고 계산된 쿼터니언으로 매트릭스를 세팅
	DirectX::SimpleMath::Quaternion _quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(_radianY, _radianX, _radianZ);

	m_Rotation = _quat;

	m_RotationTM = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation);
}

void Transform::Translate(DirectX::SimpleMath::Vector3 val)
{
	m_Position.x += val.x;
	m_Position.y += val.y;
	m_Position.z += val.z;
}

void Transform::SetTranslate(DirectX::SimpleMath::Vector3 val)
{
	m_Position.x = val.x;
	m_Position.y = val.y;
	m_Position.z = val.z;
}


_DLL void Transform::SetTransform(DirectX::SimpleMath::Vector3 pos,
											 DirectX::SimpleMath::Quaternion rot)
{
	m_Position = pos;
	m_Rotation = rot;
	m_RotationTM = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation);
}

void Transform::Start()
{
}

void Transform::Update(float dTime)
{
	//if (m_pMyObject->GetHasPhysics())
	{

	}

	m_Rotation = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_RotationTM); // 회전행렬로 현재 회전값 구함

	// Added By 최 요 환
	//m_EulerAngles = QuaternionToEuler(m_Rotation);   // 쿼터니언 값을 오일러로 변환해서 저장

	m_LocalTM =
		DirectX::SimpleMath::Matrix::CreateScale(m_Scale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_Position);

	// 부모자식 관계 업데이트
	UpdateNodeTM();

	m_WorldPosition = m_WorldTM.Translation();


}

void Transform::OnRender()
{

}

Transform* Transform::GetParentTM() const
{
	return m_pParent;
}

const DirectX::SimpleMath::Matrix& Transform::GetWorld()
{
	return m_WorldTM;
}

const DirectX::SimpleMath::Matrix& Transform::GetLocal()
{
	return m_LocalTM;
}

void Transform::SetParentTM(Transform* val)
{
	m_pParent = val;
}

void Transform::SetParentTM(GameObject* val)
{
	// Modified by 최 요 환
	Transform* _parentTF = val->GetComponent<Transform>();

	/// 자식이 자신의 부모를 설정할 때 부모도 해당 자식을 자신의 자식으로 추가해야 함. 
	// this의 부모 TF를 val->Transform 으로 설정
	m_pParent = _parentTF;

	// 부모(val->TF)의 자식을 나(this)로 설정
	_parentTF->AddChild(this);
}

_DLL void Transform::SetParentTM(DirectX::XMMATRIX* pParentTM)
{
	m_pParentWorldTM = pParentTM;
}

_DLL std::vector<Transform*>& Transform::GetChildV()
{
	return m_ChildTM_V;
}

// 자식 TM은 어떻게 쓰이나? MK1 단계에서는 없는듯함
void Transform::AddChild(Transform* child)
{
	// 예외 처리 추가
	for (Transform* _pChild : m_ChildTM_V)
	{
		if (_pChild == child)
		{
			return;
		}
	}

	m_ChildTM_V.push_back(child);
}

// 재귀를 통해 부모를 찾아 업데이트 해줌
DirectX::SimpleMath::Matrix& Transform::ParentLocalTMRecursion(Transform* TM)
{
	if (TM->m_pParent != nullptr)
	{
		return TM->m_LocalTM * ParentLocalTMRecursion(TM->m_pParent);
	}
	else
	{
		return TM->m_LocalTM;
	}
}

// 불필요한것 같음, 부모 지정해주면 updateLocalTM이 localTM을 업뎃해줌
void Transform::SetLocalTM(DirectX::SimpleMath::Matrix& val)
{
	// MyWorldTM = MyLocalTM * (Parents'LocalTM * GrandParents'LocalTM * ...)
	// MyWorldTM = MyLocalTM * Parents'WorldTM
	// MyLocalTM = MyWorldTM * Parents'WorldTM.inverse()

	m_LocalTM = val;
	UpdateNodeTM();
}

void Transform::UpdateNodeTM()
{
	if (m_pParent != nullptr) // 부모TM이 있으면
	{
		m_WorldTM = m_LocalTM * m_pParent->m_WorldTM;
	}
	else
	{
		m_WorldTM = m_LocalTM;
	}
}

DirectX::SimpleMath::Vector3 Transform::QuaternionToEuler(const DirectX::SimpleMath::Quaternion quat) const
{
	// 버그 있음..
	float sqw = quat.w * quat.w;
	float sqx = quat.x * quat.x;
	float sqy = quat.y * quat.y;
	float sqz = quat.z * quat.z;
	float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	float test = quat.x * quat.w - quat.y * quat.z;

	DirectX::SimpleMath::Vector3 v;

	/// 각도가 360을 넘어갔을 때 처리
	if (test > 0.4995f * unit)
	{
		// singularity at north pole
		v.y = 2.0f * atan2(quat.y, quat.x);
		v.x = MathHelper::Pi / 2.0f;
		v.z = 0.0f;

		return NormalizeAngles(v * 360.0f / (MathHelper::Pi * 2.0f));
	}
	if (test < -0.4995f * unit)
	{
		// singularity at south pole
		v.y = -2.0f * atan2(quat.y, quat.x);
		v.x = -MathHelper::Pi / 2.0f;
		v.z = 0.0f;

		return NormalizeAngles(v * 360.0f / (MathHelper::Pi * 2.0f));
	}

	DirectX::SimpleMath::Quaternion q = { quat.w, quat.z, quat.x, quat.y };
	v.y = static_cast<float>(atan2(2.0f * q.x * q.w + 2.0f * q.y * q.z, 1.0f - 2.0f * (q.z * q.z + q.w * q.w)));     // Yaw
	v.x = static_cast<float>(asin(2.0f * (q.x * q.z - q.w * q.y)));                             // Pitch
	v.z = static_cast<float>(atan2(2.0f * q.x * q.y + 2.0f * q.z * q.w, 1.0f - 2.0f * (q.y * q.y + q.z * q.z)));      // Roll

	return NormalizeAngles(v * 360 / (MathHelper::Pi * 2));
}

DirectX::SimpleMath::Vector3 Transform::QuaternionToEuler(const DirectX::SimpleMath::Matrix matrix) const
{
	// y축 버그가 있음..(그것뿐만이 아닐 지도..)
	DirectX::SimpleMath::Vector3 _radianAngles = { 0.0f, 0.0f, 0.0f };

	_radianAngles.x = atan2(matrix._32, matrix._33);
	_radianAngles.y = atan2(-matrix._31, sqrt(pow(matrix._32, 2) + pow(matrix._33, 2)));
	_radianAngles.z = atan2(matrix._21, matrix._11);

	DirectX::SimpleMath::Vector3 _degreeAngles =
	{
	   RadianToDegree(_radianAngles.x),
	   RadianToDegree(_radianAngles.y),
	   RadianToDegree(_radianAngles.z),
	};

	_degreeAngles = NormalizeAngles(_degreeAngles);

	return _degreeAngles;
}

/// <summary>
/// Quaternion -> euler 변환 함수(미완성)
/// x(pitch)와 z(roll)은 어떻게든 숫자를 보정해서 맞게 나오지만
/// y(yaw)의 경우 해결이 안된다...
/// </summary>
/// <returns></returns>
_DLL void Transform::QuatToEulerTest()
{
	Matrix matrix = m_RotationTM;
	Quaternion q = m_Rotation;
	Vector3 angle;

	angle.x = atan2(matrix._32, matrix._33);
	angle.y = atan2(-matrix._31, std::sqrt(matrix._32 * matrix._32 + matrix._33 * matrix._33));
	angle.z = atan2(matrix._21, matrix._11);

	angle.x = -RadianToDegree(angle.x);
	if (angle.x < 0.0f)
	{
		angle.x += 360.0f;
	}
	if (angle.x >= 360.0f)
	{
		angle.x -= 360.0f;
	}

	angle.y = -RadianToDegree(angle.y);
	if (angle.y < 0.0f && angle.y > -90.0f)
	{
		angle.y += 360.0f;
	}

	angle.z = -RadianToDegree(angle.z);
	if (angle.z < 0.0f)
	{
		angle.z += 360.0f;
	}

	/*
	// x
	double sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
	double cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);

	angle.x = std::atan2(sinr_cosp, cosr_cosp);

	// y
	double sinp = 2.0 * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1.0)
	{
		angle.y = std::copysign(MathHelper::Pi / 2.0, sinp);
	}
	else
	{
		angle.y = std::asin(sinp);
	}

	// z
	double siny_cosp = 2.0 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
	angle.z = std::atan2(siny_cosp, cosy_cosp);

	angle.x = angle.x * MathHelper::Pi / 180.0f;
	angle.y = angle.y * MathHelper::Pi / 180.0f;
	angle.z = angle.z * MathHelper::Pi / 180.0f;
	*/

	//CA_TRACE("%f / %f/ %f", angle.x, angle.y, angle.z);

	return;
}

_DLL DirectX::SimpleMath::Vector2 Transform::NormalizeAngles(DirectX::SimpleMath::Vector2 angles) const
{
	angles.x = NormalizeAngle(angles.x);
	angles.y = NormalizeAngle(angles.y);

	return angles;
}

DirectX::SimpleMath::Vector3 Transform::NormalizeAngles(DirectX::SimpleMath::Vector3 angles) const
{
	angles.x = NormalizeAngle(angles.x);
	angles.y = NormalizeAngle(angles.y);
	angles.z = NormalizeAngle(angles.z);
	return angles;
}

float Transform::NormalizeAngle(float angle) const
{
	if (angle > 360)
	{
		angle -= 360;
	}
	else if (angle < 0)
	{
		angle += 360;
	}

	return angle;
}

float Transform::RadianToDegree(float rad) const
{
	return (rad * (180.0f / MathHelper::Pi));
}

/// <summary>
/// targetPos를 향해 바라보도록 y축을 회전시는 함수
/// atan2 이용
/// </summary>
/// <param name="targetPos">타겟 위치</param>
/// <returns></returns>
_DLL void Transform::LookAtYaw(Vector3 targetPos)
{
	Vector3 myPos = m_Transform->m_Position;

	/// 타겟과의 대변(dz), 밑변(dx)로 θ를 구함 -> 으아크 탄제에에엔트 투우우우의 힘으로~
	// [참고] 게임을 움직이는 수학과 물리. [5장 각도 - atan2 의 역할] 171.p

	//	           . - targetPos
	//		      /|
	//		     / |
	//		    /  |
	//		   /   |
	//		  /    |	dz
	//		 /     |
	//		/______|
	//  myPos 
	//		   dx
	
	float dx = targetPos.x - myPos.x;
	float dz = targetPos.z - myPos.z;
	float dy = targetPos.y - myPos.y;

	float yawRad = std::atan2f(dx, dz);			// 왜 파라미터를 반대로하니까 되냐 ㅂㄷㅂㄷ
	float yawAngle = RadianToDegree(yawRad);

	float pitchRad = std::atan2f(dy, dz);
	float pitchAngle = RadianToDegree(pitchRad) / 8.0f;	// - 안붙이면 거꾸로 돌아가서 부호 반전시켜줌.. 
															// / 8.0f 는 안하면 회전이 너무 많이되는거 같아서 약간 보정해서 자연스럽게 하기 위함..

	// [x,z] 평면 위의 회전각 θ로 yaw 를 돌린다.
	m_Transform->SetRotationFromVec({ 0.0f, yawAngle, 0.0f });
	
	// pitch의 경우 y축이 같은 값의 높이에 있어도 pitch가 되는 버그가 좀 있다..
	//m_Transform->SetRotationFromVec({ pitchAngle, yawAngle, 0.0f });

}

/// <summary>
/// 
/// </summary>
/// <param name="targetPos"></param>
/// <returns></returns>
_DLL void Transform::LookAtPitchAndYaw(DirectX::SimpleMath::Vector3 targetPos)
{
	Vector3 myPos = m_Transform->m_Position;

	float dx = targetPos.x - myPos.x;
	float dz = targetPos.z - myPos.z;
	float dy = targetPos.y - myPos.y;

	float yawRad = std::atan2f(dx, dz);			// 왜 파라미터를 반대로하니까 되냐 ㅂㄷㅂㄷ
	float yawAngle = RadianToDegree(yawRad);

	float pitchRad = std::atan2f(dy, dz);
	float pitchAngle = -RadianToDegree(pitchRad);	// - 안붙이면 거꾸로 돌아가서 부호 반전시켜줌..

	// pitch의 경우 y축이 같은 값의 높이에 있어도 pitch가 되는 버그가 좀 있다..
	m_Transform->SetRotationFromVec({ pitchAngle, yawAngle, 0.0f });
}




void Transform::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_LocalTM = &m_LocalTM;
	m_SaveData->m_Scale = &m_Scale;
	m_SaveData->m_Rotation = &m_Rotation;
	m_SaveData->m_Position = &m_Position;
	m_SaveData->m_EulerAngles = &m_EulerAngles;

	//m_SaveData->m_LocalTM[0] = m_LocalTM._11;
	//m_SaveData->m_LocalTM[1] = m_LocalTM._12;
	//m_SaveData->m_LocalTM[2] = m_LocalTM._13;
	//m_SaveData->m_LocalTM[3] = m_LocalTM._14;
	//m_SaveData->m_LocalTM[4] = m_LocalTM._21;
	//m_SaveData->m_LocalTM[5] = m_LocalTM._22;
	//m_SaveData->m_LocalTM[6] = m_LocalTM._23;
	//m_SaveData->m_LocalTM[7] = m_LocalTM._24;
	//m_SaveData->m_LocalTM[8] = m_LocalTM._31;
	//m_SaveData->m_LocalTM[9] = m_LocalTM._32;
	//m_SaveData->m_LocalTM[10] = m_LocalTM._33;
	//m_SaveData->m_LocalTM[11] = m_LocalTM._34;
	//m_SaveData->m_LocalTM[12] = m_LocalTM._41;
	//m_SaveData->m_LocalTM[13] = m_LocalTM._42;
	//m_SaveData->m_LocalTM[14] = m_LocalTM._43;
	//m_SaveData->m_LocalTM[15] = m_LocalTM._44;
	//m_SaveData->m_Scale[0] = m_Scale.x;
	//m_SaveData->m_Scale[1] = m_Scale.y;
	//m_SaveData->m_Scale[2] = m_Scale.z;
	//m_SaveData->m_Rotation[0] = m_Rotation.x;
	//m_SaveData->m_Rotation[1] = m_Rotation.y;
	//m_SaveData->m_Rotation[2] = m_Rotation.z;
	//m_SaveData->m_Rotation[3] = m_Rotation.w;
	//m_SaveData->m_Position[0] = m_Position.x;
	//m_SaveData->m_Position[1] = m_Position.y;
	//m_SaveData->m_Position[2] = m_Position.z;
	//m_SaveData->m_EulerAngles[0] = m_EulerAngles.x;
	//m_SaveData->m_EulerAngles[1] = m_EulerAngles.y;
	//m_SaveData->m_EulerAngles[2] = m_EulerAngles.z;

	m_SaveData->m_pObject = m_pMyObject->GetGameObjectId();

	m_pParent != nullptr ?
		m_SaveData->m_pParent = m_pParent->GetComponetId() : m_SaveData->m_pParent = NULL;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void Transform::LoadData()
{
	m_SaveData->m_LocalTM = &m_LocalTM;
	m_SaveData->m_Scale = &m_Scale;
	m_SaveData->m_Rotation = &m_Rotation;
	m_SaveData->m_Position = &m_Position;
	m_SaveData->m_EulerAngles = &m_EulerAngles;
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);
	//m_LocalTM._11 = m_SaveData->m_LocalTM[0];
	//m_LocalTM._12 = m_SaveData->m_LocalTM[1];
	//m_LocalTM._13 = m_SaveData->m_LocalTM[2];
	//m_LocalTM._14 = m_SaveData->m_LocalTM[3];
	//m_LocalTM._21 = m_SaveData->m_LocalTM[4];
	//m_LocalTM._22 = m_SaveData->m_LocalTM[5];
	//m_LocalTM._23 = m_SaveData->m_LocalTM[6];
	//m_LocalTM._24 = m_SaveData->m_LocalTM[7];
	//m_LocalTM._31 = m_SaveData->m_LocalTM[8];
	//m_LocalTM._32 = m_SaveData->m_LocalTM[9];
	//m_LocalTM._33 = m_SaveData->m_LocalTM[10];
	//m_LocalTM._34 = m_SaveData->m_LocalTM[11];
	//m_LocalTM._41 = m_SaveData->m_LocalTM[12];
	//m_LocalTM._42 = m_SaveData->m_LocalTM[13];
	//m_LocalTM._43 = m_SaveData->m_LocalTM[14];
	//m_LocalTM._44 = m_SaveData->m_LocalTM[15];
	//m_Scale = *(m_SaveData->m_Scale);
	//m_Scale.x = m_SaveData->m_Scale[0];
	//m_Scale.y = m_SaveData->m_Scale[1];
	//m_Scale.z = m_SaveData->m_Scale[2];
	//m_Rotation.x = m_SaveData->m_Rotation[0];
	//m_Rotation.y = m_SaveData->m_Rotation[1];
	//m_Rotation.z = m_SaveData->m_Rotation[2];
	//m_Rotation.w = m_SaveData->m_Rotation[3];
	//m_Position.x = m_SaveData->m_Position[0];
	//m_Position.y = m_SaveData->m_Position[1];
	//m_Position.z = m_SaveData->m_Position[2];
	//m_EulerAngles.x = m_SaveData->m_EulerAngles[0];
	//m_EulerAngles.y = m_SaveData->m_EulerAngles[1];
	//m_EulerAngles.z = m_SaveData->m_EulerAngles[2];

	m_RotationTM = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation);
	SetRotationFromVec(m_EulerAngles);

}

void Transform::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{
	if (m_SaveData->m_pParent != NULL)
	{
		m_pParent = dynamic_cast<Transform*>(component->at(m_SaveData->m_pParent));
	}
}
