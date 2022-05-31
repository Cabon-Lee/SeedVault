#pragma once
#include "ComponentBase.h"

/// <summary>
/// ���� A.I �� �����ϴ� Ŭ����
/// 
/// BehaviorTree ����� ��带 �����ؼ� A.I�� �����Ѵ�.
/// 
/// �ۼ��� : Yoking
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

