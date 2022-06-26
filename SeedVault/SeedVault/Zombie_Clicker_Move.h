#pragma once

#include "Enemy_Move.h"

/// <summary>
/// Ŭ��Ŀ ���� �̵� Ŭ����
/// 
/// �Ҹ��� ����(�þ� X)
/// �÷��̾ Ŭ��Ŀ�� �����ϸ� ���� ����
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>

class Zombie_Clicker_Move : public Enemy_Move
{
public:
	Zombie_Clicker_Move();
	virtual ~Zombie_Clicker_Move();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	virtual void OnCollisionEnter(Collision collision) override;
	virtual void OnCollisionStay(Collision collision) override;
	virtual void OnCollisionExit(Collision collision) override;

	// Behavior Tree Node�� �����Ǵ� �Լ�
	virtual bool IsDead() override;								// �׾����� �Ǵ�
	virtual bool IsAttackPlayer() override;						// �÷��̾� �������ΰ� �Ǵ�

	virtual bool MoveToReturnPoint() override;					// ��ȯ ��ġ�� �̵�
};

