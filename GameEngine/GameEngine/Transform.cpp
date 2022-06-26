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

	//m_LocalTM = m_WorldTM; // �����ÿ��� �θ� ���� �����̹Ƿ� ������ǥ�� ����
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

	// ������ ���Ͱ� ������ ������ ������(Ʈ������)�� ����ȭ�Ѵ�.
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

	// Modified by �� �� ȯ
	// SetRotation() �� �Ẹ��!!
	// �Ķ���ͷ� ���� ���ʹϾ�(rot)�� m_RotationTM �� ����
	m_RotationTM = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation);

	m_EulerAngles = QuaternionToEuler(m_Rotation);   // ���ʹϾ� ���� ���Ϸ��� ��ȯ�ؼ� ����(���� ����..)
}

// Added by �� �� ȯ
_DLL void Transform::SetRotationFromVec(const DirectX::SimpleMath::Vector2& rot)
{
	// �� �����(World ����)���� �����Ѵ�

	// Added By �� �� ȯ
	m_EulerAngles.x = rot.x;
	m_EulerAngles.y = rot.y;

	SetRotationFromVec(m_EulerAngles);
}

// Added by �� �� ȯ
_DLL void Transform::SetRotationFromVec(const DirectX::SimpleMath::Vector3& rot)
{
	// �� �����(World ����)���� �����Ѵ�

	// Added By �� �� ȯ
	m_EulerAngles = rot;
	m_EulerAngles = NormalizeAngles(m_EulerAngles);	// 0 ~ 360 ���̷� ����

	// Degree -> Radian
	float _radianX = m_EulerAngles.x * MathHelper::Pi / 180;
	float _radianY = m_EulerAngles.y * MathHelper::Pi / 180;
	float _radianZ = m_EulerAngles.z * MathHelper::Pi / 180;

	// ���ʹϾ��� ����� ���� ���ʹϾ����� ��Ʈ������ ����
	DirectX::SimpleMath::Quaternion _quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(_radianY, _radianX, _radianZ);

	m_Rotation = _quat;
	m_RotationTM = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation);

	// ������ ���Ͱ� ������Ʈ�� ������
	if (m_pMyObject->GetComponent<PhysicsActor>())
	{
		// ������ ���� ���Ϳ��� ����� ����(Ʈ������) ��������
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

	// �θ��ڽ� ���� ������Ʈ
	UpdateNodeTM();

	m_WorldPosition = m_WorldTM.Translation();

	// ������ ���Ͱ� ������ ������ ������(Ʈ������)�� ����ȭ�Ѵ�.
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

// ������ ���ư������� 'ȸ����' Offset�� ������
_DLL void Transform::MoveForward(float val)
{
	DirectX::SimpleMath::Vector3 _forward = m_RotationTM.Forward();
	_forward.Normalize();
	m_Position += _forward * val; // simpleMath�� ��ǥ��� rightHand��� �ݴ�� ����
}

_DLL void Transform::MoveSide(float val)
{
	DirectX::SimpleMath::Vector3 _side = m_RotationTM.Right();
	_side.Normalize();
	m_Position += _side * val;
}

// ȸ���ϱ� ���� �Լ���
// Modified by �� �� ȯ
_DLL void Transform::RotateY(float val)
{
	//(��)
	//DirectX::SimpleMath::Matrix _rotTM = DirectX::XMMatrixRotationAxis({ 0.0f, 1.0f, 0.0f }, val); // Y������ ������ŭ ȸ����� ����
	//m_RotationTM *= _rotTM;                                     // ȸ�� ������
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// Modified by �� �� ȯ
	m_EulerAngles.y += val;

	// Degree -> Radian
	float _radianX = m_EulerAngles.x * MathHelper::Pi / 180;
	float _radianY = m_EulerAngles.y * MathHelper::Pi / 180;
	float _radianZ = m_EulerAngles.z * MathHelper::Pi / 180;

	// ���ʹϾ��� ����� ���� ���ʹϾ����� ��Ʈ������ ����
	DirectX::SimpleMath::Quaternion _quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(_radianY, _radianX, _radianZ);

	m_Rotation = _quat;

	m_RotationTM = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation);
}

_DLL void Transform::Pitch(float val)
{
	// (��)
	//DirectX::SimpleMath::Matrix _rotTM = DirectX::XMMatrixRotationAxis(m_LocalTM.Right(), val); // �þ��� ������ ������ ������ŭ ȸ����� ����
	//m_RotationTM *= _rotTM;                                        // ȸ�� ������
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// Modified By �� �� ȯ
	m_EulerAngles.x += val;

	// Degree -> Radian
	float _radianX = m_EulerAngles.x * MathHelper::Pi / 180;
	float _radianY = m_EulerAngles.y * MathHelper::Pi / 180;
	float _radianZ = m_EulerAngles.z * MathHelper::Pi / 180;

	// ���ʹϾ��� ����� ���� ���ʹϾ����� ��Ʈ������ ����
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

	m_Rotation = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_RotationTM); // ȸ����ķ� ���� ȸ���� ����

	// Added By �� �� ȯ
	//m_EulerAngles = QuaternionToEuler(m_Rotation);   // ���ʹϾ� ���� ���Ϸ��� ��ȯ�ؼ� ����

	m_LocalTM =
		DirectX::SimpleMath::Matrix::CreateScale(m_Scale) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation) *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_Position);

	// �θ��ڽ� ���� ������Ʈ
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
	// Modified by �� �� ȯ
	Transform* _parentTF = val->GetComponent<Transform>();

	/// �ڽ��� �ڽ��� �θ� ������ �� �θ� �ش� �ڽ��� �ڽ��� �ڽ����� �߰��ؾ� ��. 
	// this�� �θ� TF�� val->Transform ���� ����
	m_pParent = _parentTF;

	// �θ�(val->TF)�� �ڽ��� ��(this)�� ����
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

// �ڽ� TM�� ��� ���̳�? MK1 �ܰ迡���� ���µ���
void Transform::AddChild(Transform* child)
{
	// ���� ó�� �߰�
	for (Transform* _pChild : m_ChildTM_V)
	{
		if (_pChild == child)
		{
			return;
		}
	}

	m_ChildTM_V.push_back(child);
}

// ��͸� ���� �θ� ã�� ������Ʈ ����
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

// ���ʿ��Ѱ� ����, �θ� �������ָ� updateLocalTM�� localTM�� ��������
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
	if (m_pParent != nullptr) // �θ�TM�� ������
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
	// ���� ����..
	float sqw = quat.w * quat.w;
	float sqx = quat.x * quat.x;
	float sqy = quat.y * quat.y;
	float sqz = quat.z * quat.z;
	float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	float test = quat.x * quat.w - quat.y * quat.z;

	DirectX::SimpleMath::Vector3 v;

	/// ������ 360�� �Ѿ�� �� ó��
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
	// y�� ���װ� ����..(�װͻӸ��� �ƴ� ����..)
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
/// Quaternion -> euler ��ȯ �Լ�(�̿ϼ�)
/// x(pitch)�� z(roll)�� ��Ե� ���ڸ� �����ؼ� �°� ��������
/// y(yaw)�� ��� �ذ��� �ȵȴ�...
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
/// targetPos�� ���� �ٶ󺸵��� y���� ȸ���ô� �Լ�
/// atan2 �̿�
/// </summary>
/// <param name="targetPos">Ÿ�� ��ġ</param>
/// <returns></returns>
_DLL void Transform::LookAtYaw(Vector3 targetPos)
{
	Vector3 myPos = m_Transform->m_Position;

	/// Ÿ�ٰ��� �뺯(dz), �غ�(dx)�� �踦 ���� -> ����ũ ź��������Ʈ �������� ������~
	// [����] ������ �����̴� ���а� ����. [5�� ���� - atan2 �� ����] 171.p

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

	float yawRad = std::atan2f(dx, dz);			// �� �Ķ���͸� �ݴ���ϴϱ� �ǳ� ��������
	float yawAngle = RadianToDegree(yawRad);

	float pitchRad = std::atan2f(dy, dz);
	float pitchAngle = RadianToDegree(pitchRad) / 8.0f;	// - �Ⱥ��̸� �Ųٷ� ���ư��� ��ȣ ����������.. 
															// / 8.0f �� ���ϸ� ȸ���� �ʹ� ���̵Ǵ°� ���Ƽ� �ణ �����ؼ� �ڿ������� �ϱ� ����..

	// [x,z] ��� ���� ȸ���� ��� yaw �� ������.
	m_Transform->SetRotationFromVec({ 0.0f, yawAngle, 0.0f });
	
	// pitch�� ��� y���� ���� ���� ���̿� �־ pitch�� �Ǵ� ���װ� �� �ִ�..
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

	float yawRad = std::atan2f(dx, dz);			// �� �Ķ���͸� �ݴ���ϴϱ� �ǳ� ��������
	float yawAngle = RadianToDegree(yawRad);

	float pitchRad = std::atan2f(dy, dz);
	float pitchAngle = -RadianToDegree(pitchRad);	// - �Ⱥ��̸� �Ųٷ� ���ư��� ��ȣ ����������..

	// pitch�� ��� y���� ���� ���� ���̿� �־ pitch�� �Ǵ� ���װ� �� �ִ�..
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
