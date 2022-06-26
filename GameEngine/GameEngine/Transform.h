  #pragma once
#include "DLLDefine.h"

#include "SimpleMath.h"

#include "ComponentBase.h"

/// 오브젝트의 Transform과 관련된 클래스 component 상속 

struct Transform_Save;

class Transform : public ComponentBase
{
public:
	_DLL Transform(); // 암시적 링크라 _DLL 필요한 듯 하다 알아봐야함
	_DLL virtual ~Transform();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	_DLL const DirectX::SimpleMath::Vector3& GetScale() const;
	_DLL void SetScale(const DirectX::SimpleMath::Vector3& scl);
	_DLL const DirectX::SimpleMath::Quaternion& GetRotation() const;
	_DLL void SetRotation(const DirectX::SimpleMath::Quaternion& rot);
	_DLL void SetRotationFromVec(const DirectX::SimpleMath::Vector2& rot);   // Added by 최 요 환
	_DLL void SetRotationFromVec(const DirectX::SimpleMath::Vector3& rot);   // Added by 최 요 환
	_DLL const DirectX::SimpleMath::Vector3& GetLocalPosition() const;
	_DLL void SetPosition(const DirectX::SimpleMath::Vector3& pos);
	_DLL const DirectX::SimpleMath::Vector3& GetWorldPosition() const;

	_DLL void MoveForwardOnXZ(float val);
	_DLL void MoveForward(float val);
	_DLL void MoveSide(float val);
	_DLL void RotateY(float val);
	_DLL void Pitch(float val);

	_DLL void Translate(DirectX::SimpleMath::Vector3 val); // 월드 기준으로 이동
	_DLL void SetTranslate(DirectX::SimpleMath::Vector3 val); // 월드 기준으로 이동
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
	DirectX::SimpleMath::Matrix m_RotationTM; // 축회전 offset용 TM

	// 아래 변수로 transform
	DirectX::SimpleMath::Vector3 m_Scale;
	DirectX::SimpleMath::Quaternion m_Rotation;
	DirectX::SimpleMath::Vector3 m_Position;

	DirectX::SimpleMath::Vector3 m_WorldPosition;

	// Modified by 최 요 환
	// 임시로 여기에 만들고 나중에 개선... 일단 되는지 확인
	// Quaternion 을 Euler로 변환하기 위한 함수 -> 안됨...
	DirectX::SimpleMath::Vector3 QuaternionToEuler(const DirectX::SimpleMath::Quaternion quat) const;  // 특정 범위에선 올바른 값을 가지나 특정 범위에서 값이 이상해짐....
	DirectX::SimpleMath::Vector3 QuaternionToEuler(const DirectX::SimpleMath::Matrix matrix) const;  //  오차가 있을 수 있지만 정상! 행렬을 오일러로 변환하는 이 함수를 사용!
	_DLL void QuatToEulerTest();

	_DLL DirectX::SimpleMath::Vector2 NormalizeAngles(DirectX::SimpleMath::Vector2 angles) const;
	_DLL DirectX::SimpleMath::Vector3 NormalizeAngles(DirectX::SimpleMath::Vector3 angles) const;

	float NormalizeAngle(float angle) const;
	float RadianToDegree(float rad) const;

	DirectX::SimpleMath::Vector3 m_EulerAngles;		// 오일러 각

	_DLL void LookAtYaw(DirectX::SimpleMath::Vector3 targetPos);	// 타겟 방향으로 바라보게 만드는 함수(yaw 값만 적용 pitch, roll 은 0.0f 고정)
	_DLL void LookAtPitchAndYaw(DirectX::SimpleMath::Vector3 targetPos);	// 타겟 방향으로 바라보게 만드는 함수(yaw 값만 적용 pitch, roll 은 0.0f 고정)
	//_DLL void LookAt(Vector3 target); // 추후에 Transform target 을 받는 함수를 만들지도..
	

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
/// 저장용 구조체
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
