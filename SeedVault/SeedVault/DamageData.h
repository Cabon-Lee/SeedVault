#pragma once

/// <summary>
/// ������ ���� �����̳� Ŭ����
/// 
/// ������ ��, ���� ��ü ���� ���� ������.
/// 
/// �ۼ��� : YoKing
/// </summary>

class DamageData
{
public:
	DamageData();
	virtual ~DamageData();

	float GetAmount() const;
	Vector3 GetPosition() const;
	Vector3 GetDirection() const;
	float GetForceMagnitude() const;
	GameObject* GetOwner() const;
	PhysicsActor* GetPhysicsActor() const;

	void SetDamage(float amount, Vector3 pos, Vector3 dir, float magnitude, GameObject* attacker, PhysicsActor* hitPhysicsActor);

protected:
	float m_Amount;						// The amount of damage taken
	Vector3 m_Position;					// The position of the damage.
	Vector3 m_Direction;				// The direction that the object took damage from
	float m_ForceMagnitude;				// The magnitude of the force that is applied to the object
	GameObject* m_Owner;				// The GameObject that did the damage
	PhysicsActor* m_HitPhysicsActor;	// The Collider that was hit
	


private:

};