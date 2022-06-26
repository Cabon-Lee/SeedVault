#pragma once

#include "Enemy_Move.h"

/// <summary>
/// 클리커 좀비 이동 클래스
/// 
/// 소리만 감지(시야 X)
/// 플레이어가 클리커와 접촉하면 게임 오버
/// 
/// 작성자 : 최 요 환
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

	// Behavior Tree Node에 대응되는 함수
	virtual bool IsDead() override;								// 죽었는지 판단
	virtual bool IsAttackPlayer() override;						// 플레이어 공격중인가 판단

	virtual bool MoveToReturnPoint() override;					// 귀환 위치로 이동
};

