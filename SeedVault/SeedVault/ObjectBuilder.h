#pragma once
/// <summary>
/// 원하는 오브젝트 타입을 받아서 
/// 초기세팅(컴포넌트 추가, 값 세팅)을 해주는 클래스
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

