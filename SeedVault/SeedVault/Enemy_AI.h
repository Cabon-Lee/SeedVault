#pragma once
#include "ComponentBase.h"

/// <summary>
/// 적의 A.I 를 구성하는 클래스
/// 
/// BehaviorTree 기반의 노드를 생성해서 A.I를 구성한다.
/// 
/// 작성자 : Yoking
/// </summary>

class Enemy_AI : public ComponentBase
{
public:
	Enemy_AI();
	virtual ~Enemy_AI() = default;

	virtual void Start() abstract;
	virtual void Update(float dTime) abstract;
	virtual void OnRender() abstract;

protected:
	class Enemy_Move* m_Enemy;
};

