#pragma once
#include "DLLDefine.h"
#include "ComponentBase.h"
#include "EffectUIEnum.h"

__interface IRenderer;
__interface IResourceManager;

class Transform;
struct Actor;

/// <summary>
/// 트랜스폼 애니메이션 컴포넌트
///	 : 매쉬 이동, 회전 시킬 수 있는 애니메이션
///	   	
/// 2022.02.12 B.Bunny
/// </summary>
class TransformAnimation : public ComponentBase
{
public:
	_DLL TransformAnimation();
	_DLL virtual ~TransformAnimation();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override {};	

	_DLL void SetPlayType(ePlayType playType);
	_DLL void SetFrameRate(eFrameRate frameRate);
	_DLL void AddActor(GameObject* gameObject);
	_DLL Actor* GetActor(std::string gameObjectName);
	
	_DLL void Reset();
	_DLL void Play();
	
private:
	void SetKeyFrameInfo();
	void SetPlayTime();

private:
	ePlayType m_playType;			// 한번만 재생할지, 루프할지
	eFrameRate m_FrameRate;			// fps

	float m_fps;					// fps의 수(24, 30, 60...)
	float m_playTime;				// 애니메이션의 재생 길이
	float m_timeStack;				// 흐른 시간
	bool m_isPlay;					// 애니메이션 정지, 재생 제어
	
	std::vector<Actor*> m_actor_V;

};

struct KeyFrame
{
	KeyFrame() {};
	KeyFrame(float dur, 
		unsigned int frame, 
		const DirectX::SimpleMath::Matrix& tm)
		: duration(dur), frame(frame), keyMatrix(tm) {};

	KeyFrame(float dur,
		unsigned int frame)
		: duration(dur), frame(frame) {};

	void TransformUpdate(Transform* transform);
	void TransformUpdate(Transform* transform, DirectX::SimpleMath::Vector3 pos);
	void TransformUpdate(Transform* transform, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 rot);

	float duration;							// 한 키프레임의 재생시간
	unsigned int frame;						// 키프레임의 프레임

	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion rotation;
	DirectX::SimpleMath::Vector3 position;

	DirectX::SimpleMath::Vector3 changeScale;
	DirectX::SimpleMath::Vector3 changeRotation;
	DirectX::SimpleMath::Vector3 changePosition;

	DirectX::SimpleMath::Matrix keyMatrix;
};

struct Actor
{
	Actor() {};
	Actor(GameObject* pGO);

	_DLL void AddKeyFrame(float dur, DirectX::SimpleMath::Vector3 pos);
	_DLL void AddKeyFrame(float dur, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 rot);
	_DLL void AddKeyFrame(float dur, DirectX::SimpleMath::Matrix mat);

	void OriginTransformUpdata();

	unsigned int currKeyframe;	// 현재 키프레임
	unsigned int endFrame;		// 엑터가 끝나는 프레임
	GameObject* pGameObject;	// 움직일 대상 오브젝트
	//DirectX::SimpleMath::Quaternion currRot;
	DirectX::SimpleMath::Matrix originTM;

	std::vector<KeyFrame> keyFrame_V;
};