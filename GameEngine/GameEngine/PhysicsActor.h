#pragma once
#include "PhysicsGeometry.h"
#include "ComponentBase.h"

enum CollisionState;
__interface IRenderer;
struct CollisionHit;
class Collision;

enum RigidType
{
	/// Ÿ���� 0�̸� ��������
	Dynamic = 1, // �������� ������?
	Static // Transform�� �ٲ����ʴ��� ���������ʴ� static ��ü
};

struct BoundingOffset
{
	float Forward;
	float Right;
	float Bottom;
};


/// ������ų ��
struct FreezePositionAndRotation
{
	struct Position
	{
		bool x;
		bool y;
		bool z;

		Position()
			: x(false)
			, y(false)
			, z(false)
		{};

	}position;

	struct Rotation
	{
		bool x;
		bool y;
		bool z;

		Rotation()
			: x(true)
			, y(true)
			, z(true)
		{};

	}rotation;
};

/// <summary>
/// �̰� ������ �־���մϴ�!
/// 
/// 2021.12.28 ������
/// </summary>
class PhysicsActor : public ComponentBase
{
public:
	PhysicsActor();
	_DLL PhysicsActor(float radius, RigidType type); // for sphere shape
	_DLL PhysicsActor(DirectX::SimpleMath::Vector3 box, RigidType type); // for box shape
	_DLL PhysicsActor(float radius, float height, RigidType type); // for capsule shape
	_DLL virtual ~PhysicsActor();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;
	virtual void OnHelperRender() override;

	virtual void OnEnable() override;
	virtual void OnDisable() override;


	// ���� transform�� �ٲٴ� ���̳� �̸� ������������ ������ PxTransform�� ���Խ����ָ� �ǳ�?
	void SetPhysicalTransform(Transform* transform);

	_DLL RigidType GetRigidType() const;
	_DLL GeometryType GetGetGeometryType() const;
	_DLL GeometryBase* GetGeometry() const;

	std::shared_ptr<IRenderer> m_pIRenderer;

	unsigned int GetUserIndex() const;
	void SetUserIndex(int val);

	static int s_UserIndex;

	// �� �� ȯ's Test
	void release();
	_DLL void SetKinematic(const bool val);
	_DLL bool IsKinematic() const;

	_DLL void SetGravity(const bool val);
	_DLL bool IsGravity() const;

	// ���� �ݶ��̴�(PxShape)�� ����� �� box �ݶ��̴��� ����� ����
	_DLL void SetBoxCollider(const Vector3 box, const RigidType rigidType);
	_DLL void SetSphereCollider(const float radius, const RigidType rigidType);

	void SetBoundingOffset();
	_DLL struct BoundingOffset& GetBoundingOffset();


	void ReserveCollisionVec();
	std::vector<Collision> m_CollisionEnter_V;	// �浹 ���� ���� ����Ʈ
	std::vector<Collision> m_CollisionStay_V;	// �浹 ���� ���� ����Ʈ
	std::vector<Collision> m_CollisionExit_V;	// �浹 ���� ���� ����Ʈ

	_DLL FreezePositionAndRotation& GetFreezePositionAndRotation();

	_DLL FreezePositionAndRotation::Position& GetFreezePosition();
	_DLL void SetFreezePosition(bool x, bool y, bool z);
	
	_DLL FreezePositionAndRotation::Rotation& GetFreezeRotation();
	_DLL void SetFreezeRotation(bool x, bool y, bool z);

	_DLL void SetVelocity(const Vector3& dir);

public:
	virtual void SaveData();
	virtual void LoadData();
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);

	void CreateGeometry(GeometryType type);

	struct PhysicsActor_Save* m_SaveData;



private:
	DirectX::SimpleMath::Matrix m_PhysicsActorWorldTM;

	// ������ ���Ϳ� ������ �ε���
	unsigned int m_UserIndex;
	//unsigned int m_PhysActorIndex;

	GeometryType m_GeometryType;
	RigidType m_RigidType; // ��ü Ÿ��
	GeometryBase* m_pGeometry; // ������ ����, ���

	BoundingOffset m_BoundingOffset;


	bool m_bKinematic;		// Ű�׸��� ����(true�� ���� �̵� �Ұ�)
	bool m_bGravity;		// �߷� ���� ���� ����

	FreezePositionAndRotation m_FreezePositionAndRotation;	// pos, rot ���� ����
};


struct PhysicsActor_Save
{
	bool			m_bEnable;
	unsigned int	m_ComponentId;

	int s_UserIndex;

	unsigned int m_UserIndex;
	//unsigned int m_PhysActorIndex;

	int m_GeometryType;
	int m_RigidType; // ��ü Ÿ��

	// Save/Load�� Geometry ������
	float m_SaveBoxSizeX;
	float m_SaveBoxSizeY;
	float m_SaveBoxSizeZ;
	float m_SaveRadius;

	bool m_bKinematic;
	bool m_bGravity;

	bool m_FreezePositionX;
	bool m_FreezePositionY;
	bool m_FreezePositionZ;
	bool m_FreezeRotationX;
	bool m_FreezeRotationY;
	bool m_FreezeRotationZ;
};

BOOST_DESCRIBE_STRUCT(PhysicsActor_Save, (), (
	m_bEnable,
	m_ComponentId,

	s_UserIndex,

	m_UserIndex,
	//m_PhysActorIndex,

	m_GeometryType,
	m_RigidType,

	m_SaveBoxSizeX,
	m_SaveBoxSizeY,
	m_SaveBoxSizeZ,
	m_SaveRadius,

	m_bKinematic,
	m_bGravity,

	m_FreezePositionX,
	m_FreezePositionY,
	m_FreezePositionZ,
	m_FreezeRotationX,
	m_FreezeRotationY,
	m_FreezeRotationZ

	))
