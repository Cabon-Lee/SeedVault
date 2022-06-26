  #pragma once
#include "DLLDefine.h"

#include "SimpleMath.h"

#include "ComponentBase.h"

/// ������Ʈ�� Transform�� ���õ� Ŭ���� component ��� 

struct Transform_Save;

class Transform : public ComponentBase
{
public:
	_DLL Transform(); // �Ͻ��� ��ũ�� _DLL �ʿ��� �� �ϴ� �˾ƺ�����
	_DLL virtual ~Transform();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	_DLL const DirectX::SimpleMath::Vector3& GetScale() const;
	_DLL void SetScale(const DirectX::SimpleMath::Vector3& scl);
	_DLL const DirectX::SimpleMath::Quaternion& GetRotation() const;
	_DLL void SetRotation(const DirectX::SimpleMath::Quaternion& rot);
	_DLL void SetRotationFromVec(const DirectX::SimpleMath::Vector2& rot);   // Added by �� �� ȯ
	_DLL void SetRotationFromVec(const DirectX::SimpleMath::Vector3& rot);   // Added by �� �� ȯ
	_DLL const DirectX::SimpleMath::Vector3& GetLocalPosition() const;
	_DLL void SetPosition(const DirectX::SimpleMath::Vector3& pos);
	_DLL const DirectX::SimpleMath::Vector3& GetWorldPosition() const;

	_DLL void MoveForwardOnXZ(float val);
	_DLL void MoveForward(float val);
	_DLL void MoveSide(float val);
	_DLL void RotateY(float val);
	_DLL void Pitch(float val);

	_DLL void Translate(DirectX::SimpleMath::Vector3 val); // ���� �������� �̵�
	_DLL void SetTranslate(DirectX::SimpleMath::Vector3 val); // ���� �������� �̵�
	_DLL void SetTransform(DirectX::SimpleMath::Vector3 pos,
		DirectX::SimpleMath::Quaternion rot);

	_DLL void SetParentTM(DirectX::XMMATRIX* pParentTM);
	Transform* GetParentTM() const;
	DirectX::SimpleMath::Matrix& ParentLocalTMRecursion(Transform* parentTM);
	_DLL void SetLocalTM(DirectX::SimpleMath::Matrix& val);
	_DLL const DirectX::SimpleMath::Matrix& GetWorld();
	_DLL const DirectX::SimpleMath::Matrix& GetLocal();

	void SetParentTM(Transform* val);
	_DLL void SetParentTM(GameObject* val);
	_DLL std::vector<Transform*>& GetChildV();
	void AddChild(Transform* child);

	void UpdateNodeTM();

	DirectX::SimpleMath::Matrix m_LocalTM;
	DirectX::SimpleMath::Matrix m_WorldTM;
	DirectX::SimpleMath::Matrix m_RotationTM; // ��ȸ�� offset�� TM

	// �Ʒ� ������ transform
	DirectX::SimpleMath::Vector3 m_Scale;
	DirectX::SimpleMath::Quaternion m_Rotation;
	DirectX::SimpleMath::Vector3 m_Position;

	DirectX::SimpleMath::Vector3 m_WorldPosition;

	// Modified by �� �� ȯ
	// �ӽ÷� ���⿡ ����� ���߿� ����... �ϴ� �Ǵ��� Ȯ��
	// Quaternion �� Euler�� ��ȯ�ϱ� ���� �Լ� -> �ȵ�...
	DirectX::SimpleMath::Vector3 QuaternionToEuler(const DirectX::SimpleMath::Quaternion quat) const;  // Ư�� �������� �ùٸ� ���� ������ Ư�� �������� ���� �̻�����....
	DirectX::SimpleMath::Vector3 QuaternionToEuler(const DirectX::SimpleMath::Matrix matrix) const;  //  ������ ���� �� ������ ����! ����� ���Ϸ��� ��ȯ�ϴ� �� �Լ��� ���!
	_DLL void QuatToEulerTest();

	_DLL DirectX::SimpleMath::Vector2 NormalizeAngles(DirectX::SimpleMath::Vector2 angles) const;
	_DLL DirectX::SimpleMath::Vector3 NormalizeAngles(DirectX::SimpleMath::Vector3 angles) const;

	float NormalizeAngle(float angle) const;
	float RadianToDegree(float rad) const;

	DirectX::SimpleMath::Vector3 m_EulerAngles;		// ���Ϸ� ��

	_DLL void LookAtYaw(DirectX::SimpleMath::Vector3 targetPos);	// Ÿ�� �������� �ٶ󺸰� ����� �Լ�(yaw ���� ���� pitch, roll �� 0.0f ����)
	_DLL void LookAtPitchAndYaw(DirectX::SimpleMath::Vector3 targetPos);	// Ÿ�� �������� �ٶ󺸰� ����� �Լ�(yaw ���� ���� pitch, roll �� 0.0f ����)
	//_DLL void LookAt(Vector3 target); // ���Ŀ� Transform target �� �޴� �Լ��� ��������..
	

private:
	Transform* m_pParent;
	std::vector<Transform*> m_ChildTM_V;

	DirectX::XMMATRIX* m_pParentWorldTM;


public:
	virtual void SaveData();
	virtual void LoadData();
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);

public:
	Transform_Save* m_SaveData;
};

/// <summary>
/// ����� ����ü
/// </summary>
struct Transform_Save
{
	bool			m_bEnable;
	unsigned int	m_ComponentId;

	DirectX::SimpleMath::Matrix* m_LocalTM;
	DirectX::SimpleMath::Vector3* m_Scale;
	DirectX::SimpleMath::Quaternion* m_Rotation;
	DirectX::SimpleMath::Vector3* m_Position;

	DirectX::SimpleMath::Vector3* m_EulerAngles;
	unsigned int m_pParent = NULL;		///Parent ComponentID
	unsigned int m_pObject = NULL;		///Parent ObjectID
};

BOOST_DESCRIBE_STRUCT(Transform_Save, (), (
	m_bEnable,
	m_ComponentId,
	m_LocalTM,
	m_Scale,
	m_Rotation,
	m_Position,
	m_EulerAngles,
	m_pParent,
	m_pObject
	))
