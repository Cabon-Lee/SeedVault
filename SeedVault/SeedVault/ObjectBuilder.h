#pragma once
/// <summary>
/// ���ϴ� ������Ʈ Ÿ���� �޾Ƽ� 
/// �ʱ⼼��(������Ʈ �߰�, �� ����)�� ���ִ� Ŭ����
/// </summary>

class ObjectBuilder
{
public:
	enum class ObjectType
	{
		ePlayer,
		eRightHand,
		ePartner,
		eZombie_Runner,
		eWayPoint,
		eInGameUI,
		ePauseUI		
	};

	static GameObject* MakeObject(ObjectType type);

private:
	static GameObject* MakePlayer(GameObject* obj);
	static GameObject* MakeRightHand(GameObject* obj);
	static GameObject* MakePartner(GameObject* obj);
	static GameObject* MakeZombie_Runner(GameObject* obj);
	static GameObject* MakeWayPoint(GameObject* obj);
	static GameObject* MakeIngameUI(GameObject* obj);
	static GameObject* MakePauseUI(GameObject* obj);
};

